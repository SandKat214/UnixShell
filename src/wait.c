#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "jobs.h"
#include "params.h"
#include "parser.h"
#include "wait.h"

int
wait_on_fg_pgid(pid_t const pgid)
{
  pid_t neg_pgid = -pgid;
  pid_t ushellpgid;

  if (pgid < 0) return -1;
  jid_t const jid = jobs_get_jid(pgid);
  if (jid < 0) return -1;
  /* Make sure the foreground group is running */
  /* send the "continue" signal to the process group 'pgid'
   * XXX review kill(2)
   */
  if (kill(neg_pgid, SIGCONT) < 0) return -1;

  if((ushellpgid = tcgetpgrp(STDIN_FILENO)) < 0) return -1;
    
  if (is_interactive) {
    /* Make 'pgid' the foreground process group
     * XXX review tcsetpgrp(3) */
    if (tcsetpgrp(STDIN_FILENO, pgid) < 0) return -1;
  }

  /* XXX From this point on, all exit paths must account for setting unixshell
   * back to the foreground process group--no naked return statements */
  int retval = 0;

  /* XXX Notice here we loop until ECHILD and we use the status of
   * the last child process that terminated (in the previous iteration).
   * Consider a pipeline,
   *        cmd1 | cmd2 | cmd3
   *
   * We will loop exactly 4 times, once for each child process, and a
   * fourth time to see ECHILD.
   */
  for (;;) {
    /** Wait on ALL processes in the process group 'pgid'
     * 
     * Source: waitpid(2)
     */
    int status;
    pid_t res = waitpid(neg_pgid, &status, WUNTRACED);
    if (res < 0) {
      /* Error occurred (some errors are ok, see below)
       *
       * XXX status may have a garbage value, use last_status from the
       * previous loop iteration */
      if (errno == ECHILD) {
        /* No unwaited-for children. The job is done! */
        errno = 0;
        if (jobs_get_status(jid, &status) < 0) goto err;
        if (WIFEXITED(status)) {
          /* set params.status to the correct value */
          params.status = WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
          /** set params.status to the correct value
           * 
           * Source: https://www.gnu.org/software/bash/manual/html_node/Exit-Status.html
           */
          params.status = 128 + WTERMSIG(status);
        }

        /* Remove the job for this group from the job list
         *  see jobs.h
         */
        jobs_remove_pgid(pgid);
        goto out;
      }
      goto err; /* An actual error occurred */
    }
    assert(res > 0);
    /* status is valid */

    /* Record status for reporting later when we see ECHILD */
    if (jobs_set_status(jid, status) < 0) goto err;

    /* Handle case where a child process is stopped
     *  The entire process group is placed in the background
     */
    if (WIFSTOPPED(status)) {
      fprintf(stderr, "[%jd] Stopped\n", (intmax_t)jid);
      goto out;
    }

    /* A child exited, but others remain. Loop! */
  }

out:
  if (0) {
  err:
    retval = -1;
  }

  if (is_interactive) {
    /* Make unixshell the foreground process group again
     * XXX review tcsetpgrp(3)
     *
     * Note: this will cause unixshell to receive a SIGTTOU signal.
     *       You need to also finish signal.c to have full functionality here.
     *       Otherwise you unixshell will get stopped.
     */
    if (tcsetpgrp(STDIN_FILENO, ushellpgid) < 0) return -1;
  }
  return retval;
}

/* XXX DO NOT MODIFY XXX */
int
wait_on_fg_job(jid_t jid)
{
  pid_t pgid = jobs_get_pgid(jid);
  if (pgid < 0) return -1;
  return wait_on_fg_pgid(pgid);
}

int
wait_on_bg_jobs()
{
  size_t job_count = jobs_get_joblist_size();
  struct job const *jobs = jobs_get_joblist();
  for (size_t i = 0; i < job_count; ++i) {
    pid_t pgid = jobs[i].pgid;
    jid_t jid = jobs[i].jid;
    for (;;) {
      /* Wait for process group
       * XXX make sure to do a nonblocking wait!
       */
      int status;
      pid_t pid = waitpid(pgid, &status, WNOHANG | WUNTRACED);
      if (pid == 0) {
        /* Unwaited children that haven't exited */
        break;
      } else if (pid < 0) {
        /* Error -- some errors are ok though! */
        if (errno == ECHILD) {
          /* No children -- print saved exit status */
          errno = 0;
          if (jobs_get_status(jid, &status) < 0) return -1;
          if (WIFEXITED(status)) {
            fprintf(stderr, "[%jd] Done\n", (intmax_t)jid);
          } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "[%jd] Terminated\n", (intmax_t)jid);
          }
          jobs_remove_pgid(pgid);
          job_count = jobs_get_joblist_size();
          jobs = jobs_get_joblist();
          break;
        }
        return -1; /* Other errors are not ok */
      }

      /* Record status for reporting later when we see ECHILD */
      if (jobs_set_status(jid, status) < 0) return -1;

      /* Handle case where a process in the group is stopped */
      if (WIFSTOPPED(status)) {
        fprintf(stderr, "[%jd] Stopped\n", (intmax_t)jid);
        break;
      }
    }
  }
  return 0;
}
