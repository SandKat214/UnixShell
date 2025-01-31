# UnixShell

This is a project designed to explore and understand the inner workings of Unix-based operating systems through the development of a custom command-line interface (shell). Written in C, it implements many core features typically found in Unix shells, including process creation and execution, signal handling, job control, variable management, and input parsing. It served as an educational tool to understand process management, system calls, I/O redirection, and other Unix system functions.

---

## Program Structure

It is composed of several modules that work together to implement the shell's functionality. The project uses a modular approach, with each module handling specific aspects of shell operation.

### Directory and File Overview

- **src/**: Contains the core source code of the UnixShell program. This includes implementations for built-in commands, parsing input, handling jobs, signals, and process management.

  - **builtins.c** and **builtins.h**: Contains functions for handling built-in shell commands like `cd`, `exit`, etc.
  - **exit.c** and **exit.h**: Manages the shell's exit functionality and any cleanup required before termination.
  - **expand.c** and **expand.h**: Implements the logic for expanding variables and performing substitutions in shell commands.
  - **jobs.c** and **jobs.h**: Manages background processes and job control (e.g., handling processes in the background, jobs list).
  - **params.c** and **params.h**: Handles command line arguments passed to the shell and processes them for execution.
  - **parser.c** and **parser.h**: Contains the code for parsing user input and separating commands.
  - **runner.c** and **runner.h**: Manages the execution of parsed commands, handling forking processes and invoking the appropriate executables.
  - **signal.c** and **signal.h**: Provides functionality for handling Unix signals like `SIGINT` and `SIGTERM`.
  - **unixshell.c**: The main entry point of the program; starts the shell and manages the shell's loop and interactions.
  - **vars.c** and **vars.h**: Handles shell variables, such as environment variables.
  - **wait.c** and **wait.h**: Contains logic for waiting for processes to finish (such as using `waitpid` to manage child processes).

- **makefile**: A makefile that automates the process of building the project and managing dependencies.

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

### Challenges and Solutions

Developing this unix-based shell involved addressing several technical and architectural challenges:

#### 1. **Signal Handling**
   - **Challenge**: Implementing proper signal handling for process management was complex, especially for managing the behavior of foreground and background processes.
   - **Solution**: The `sigaction` system call was utilized to configure the shell to ignore certain signals (such as `SIGINT`) in the main shell loop. The signals were restored for child processes to ensure they could handle interruptions as needed (as detailed in Kerrisk’s *The Linux Programming Interface*).
   
#### 2. **Job Control and Process Groups**
   - **Challenge**: Managing job control and ensuring that background jobs do not block foreground processes required proper handling of process groups.
   - **Solution**: The `setpgid` and `waitpid` system calls were crucial for managing job control, allowing processes to be grouped and tracked. The system ensured that background jobs could be monitored without interrupting the foreground process.

#### 3. **I/O Redirection**
   - **Challenge**: Correctly handling file descriptors for input/output redirection was necessary for commands like `echo > file.txt`.
   - **Solution**: Functions like `get_io_flags`, `move_fd`, and `do_io_redirects` utilized `open`, `dup2`, and `close` system calls to manipulate file descriptors and redirect input and output streams, ensuring seamless execution of commands with redirection.

#### 4. **Variable Management**
   - **Challenge**: Validating and expanding environment variables, particularly the handling of special characters like `$` and `*` in command arguments.
   - **Solution**: The `vars_is_valid_varname`, `vars_get`, `vars_set`, and `vars_export` functions managed the validation and assignment of variables. Additionally, the `expand_command_words` function expanded variables and special characters using custom logic.

#### 5. **Testing and Debugging**
   - **Challenge**: Debugging a system that involves numerous interconnected components was difficult, especially with the asynchronous nature of job control and process handling.
   - **Solution**: Extensive use of the debug configuration in the `Makefile`, combined with strategically placed print statements, allowed for effective debugging of various modules. Testing individual components like `builtin_cd` and `run_command_list` with specific input helped ensure the correct behavior of the shell.

### Sample of Key System Calls Used

- **`fork()`**: Used for creating child processes for executing commands.
- **`execvp()`**: Executes external commands by searching the PATH environment variable.
- **`waitpid()`**: Waits for processes to terminate or stop, ensuring proper management of foreground and background processes.
- **`setpgid()`**: Assigns processes to process groups for job control.
- **`sigaction()`**: Configures signal handling to manage interrupts and other signals.
- **`dup2()`**: Redirects input and output file descriptors for I/O redirection.

## Installation Guide

### 1. Clone the Repository
First, clone the repository to your local machine:
```bash
git clone https://github.com/SandKat214/UnixShell.git
cd UnixShell
```

### 2. Build the Project
You can build the project in different modes:

- **To create both release and debug directories:**
  ```bash
  make
  ```

- **To create only the release directory:**
  ```bash
  make release
  ```

- **To create only the debug directory:**
  ```bash
  make debug
  ```

### 3. Navigate to the Correct Directory
Once the build process is complete, navigate to the directory where the executable was created (either `release` or `debug`):

- **For release:**
  ```bash
  cd release
  ```

- **For debug:**
  ```bash
  cd debug
  ```

### 4. Run the Program
After navigating to the correct directory, you can run the UnixShell program with the following command:
```bash
./unixshell
```


