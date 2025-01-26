# UnixShell

This is a project designed to explore and understand the inner workings of Unix-based operating systems through the development of a custom command-line interface (shell). Written in C, it implements many core features typically found in Unix shells, including process creation and execution, signal handling, job control, variable management, and input parsing. It served as an educational tool to understand process management, system calls, I/O redirection, and other Unix system functions.

## Project Structure

It is composed of several modules that work together to implement the shell's functionality. The project uses a modular approach, with each module handling specific aspects of shell operation. Below is a breakdown of the key modules and methods I was responsible for implementing:

### Main Module (`bigshell.c`)

- **`main()`**: The entry point for the shell, responsible for setting up signal handling, calling the parser and runner modules, and managing the main loop.
- **Key responsibility**: Parse user input and execute commands via interactions with the parser and runner modules.

### Parser Module (`parser.c`, `parser.h`)

- **`command_list_parse()`**: Parses user commands into a structured format (`command_list`), handling argument separation, assignments, and I/O redirection.
- **Key responsibility**: Convert user input into a structured representation for further processing by the runner module.

### Runner Module (`runner.c`, `runner.h`)

- **`run_command_list()`**: Main command execution function, which handles process management, I/O redirection, and job control. It forks processes and executes commands, including built-in and external commands.
- **`expand_command_words()`**: Handles variable and special character expansion within command words.
- **`do_variable_assignment()`**: Manages variable assignments and exports variables to the environment.
- **Key responsibility**: Manage the execution of commands, handle process creation, I/O redirection, and job control.

### Signal Module (`signal.c`, `signal.h`)

- **`signal_init()`**: Initializes signal handling for the shell, setting appropriate handlers for various signals.
- **`signal_ignore()`** and **`signal_enable_interrupt()`**: Used to modify signal behavior, including ignoring interrupt signals (e.g., `Ctrl-C`).
- **Key responsibility**: Manage signal behavior within the shell, including handling interruptions and restoring default behavior.

### Built-ins Module (`builtins.c`, `builtins.h`)

- **`builtin_exit()`**: Handles the `exit` command, which terminates the shell.
- **`builtin_cd()`**: Handles the `cd` command, changing the current directory.
- **`builtin_unset()`**: Removes environment variables.
- **Key responsibility**: Implement built-in commands that modify the shell environment or control the shell’s execution.

### Vars Module (`vars.c`, `vars.h`)

- **`vars_is_valid_varname()`**: Validates variable names according to POSIX rules.
- **`vars_get()`** and **`vars_set()`**: Get and set shell variables.
- **Key responsibility**: Manage shell variables, including validation, retrieval, and modification.

### Jobs Module (`jobs.c`, `jobs.h`)

- **`jobs_add()`**: Adds a job to the shell’s job table.
- **`jobs_remove()`**: Removes a job from the job table.
- **Key responsibility**: Track and manage background jobs and job states.

### Wait Module (`wait.c`, `wait.h`)

- **`wait_on_fg_pgid()`**: Waits for all processes in the foreground process group to complete.
- **`wait_on_bg_jobs()`**: Waits for background jobs to finish without blocking the foreground.
- **Key responsibility**: Manage waiting operations for processes, ensuring that foreground and background processes are handled appropriately.

### Exit Module (`exit.c`, `exit.h`)

- **`exit_shell()`**: Handles the clean exit process for the shell.
- **Key responsibility**: Ensure proper cleanup and termination of the shell.

## Implementation Overview

I was responsible for implementing various functions across these modules to facilitate the shell’s operations:

- **Built-in Commands**: Implemented the `exit`, `cd`, and `unset` commands, ensuring they interact with the shell environment (e.g., changing directories, modifying environment variables).
- **Variable Management**: Implemented functions to validate, retrieve, set, and export shell variables, contributing to variable management functionality in the shell.
- **Signal Handling**: Implemented signal handling functions to control shell behavior, such as ignoring interrupt signals (e.g., `Ctrl-C`) to maintain smooth shell operation.
- **Job Control**: Developed functions for waiting on the completion of foreground and background jobs, crucial for the correct handling of processes.
- **Process Management**: Implemented core functions in the runner module to execute commands, including handling I/O redirection, process creation, and job control.

### Tools & Resources Used

- **The C Programming Language** by *B. Kernighan & D. Ritchie*: The project is written in C, using POSIX system calls and libraries for process management, signal handling, and variable manipulation.
- **POSIX Standard**: The POSIX specification was used to guide the correct implementation of system calls and shell behaviors.
- **Linux Man Pages & "The Linux Programming Interface"** by *M. Kerrisk*: These resources were invaluable in understanding system calls like `fork()`, `exec()`, `waitpid()`, and signal handling.

## Challenges and Solutions

Developing this unix-based shell involved addressing several technical and architectural challenges:

### 1. **Signal Handling**
   - **Challenge**: Implementing proper signal handling for process management was complex, especially for managing the behavior of foreground and background processes.
   - **Solution**: The `sigaction` system call was utilized to configure the shell to ignore certain signals (such as `SIGINT`) in the main shell loop. The signals were restored for child processes to ensure they could handle interruptions as needed (as detailed in Kerrisk’s *The Linux Programming Interface*).
   
### 2. **Job Control and Process Groups**
   - **Challenge**: Managing job control and ensuring that background jobs do not block foreground processes required proper handling of process groups.
   - **Solution**: The `setpgid` and `waitpid` system calls were crucial for managing job control, allowing processes to be grouped and tracked. The system ensured that background jobs could be monitored without interrupting the foreground process.

### 3. **I/O Redirection**
   - **Challenge**: Correctly handling file descriptors for input/output redirection was necessary for commands like `echo > file.txt`.
   - **Solution**: Functions like `get_io_flags`, `move_fd`, and `do_io_redirects` utilized `open`, `dup2`, and `close` system calls to manipulate file descriptors and redirect input and output streams, ensuring seamless execution of commands with redirection.

### 4. **Variable Management**
   - **Challenge**: Validating and expanding environment variables, particularly the handling of special characters like `$` and `*` in command arguments.
   - **Solution**: The `vars_is_valid_varname`, `vars_get`, `vars_set`, and `vars_export` functions managed the validation and assignment of variables. Additionally, the `expand_command_words` function expanded variables and special characters using custom logic.

### 5. **Testing and Debugging**
   - **Challenge**: Debugging a system that involves numerous interconnected components was difficult, especially with the asynchronous nature of job control and process handling.
   - **Solution**: Extensive use of the debug configuration in the `Makefile`, combined with strategically placed print statements, allowed for effective debugging of various modules. Testing individual components like `builtin_cd` and `run_command_list` with specific input helped ensure the correct behavior of the shell.

## Sample of Key System Calls Used

- **`fork()`**: Used for creating child processes for executing commands.
- **`execvp()`**: Executes external commands by searching the PATH environment variable.
- **`waitpid()`**: Waits for processes to terminate or stop, ensuring proper management of foreground and background processes.
- **`setpgid()`**: Assigns processes to process groups for job control.
- **`sigaction()`**: Configures signal handling to manage interrupts and other signals.
- **`dup2()`**: Redirects input and output file descriptors for I/O redirection.

## Conclusion

Building this shell served as a comprehensive learnign opportunity to demonstrate the core mechanisms of Unix-based operating systems. Through this project, I gained practical experience with system calls, process management, signal handling, and shell design principles. The implementation offers valuable insights into the foundational components of modern operating systems, which can be applied to further development and exploration in the field of system programming.
