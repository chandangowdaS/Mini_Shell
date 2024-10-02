# Mini Shell

**Author**: Chandan Gowda S  
**Date**: 30/08/2024  

## Project Description
The **Mini Shell** is a custom-built command-line shell that emulates basic shell functionalities found in Unix-based systems. It supports internal and external commands, signal handling, and piping. The shell interacts with the system kernel to execute commands and manage jobs effectively.

## Features
- **Built-in Commands**: Supports commands like `cd`, `pwd`, `echo`, and job control commands like `jobs`, `fg`, and `bg`.
- **Signal Handling**: Handles interrupts (`SIGINT`) and job suspension (`SIGTSTP`) for background job control.
- **Command Piping**: Supports execution of multiple commands through piping (`|`).
- **Prompt Customization**: Changeable prompt using the `PS1` environment variable.
- **External Commands**: Executes external commands by invoking `execvp()`.

## Internal Commands
- `cd [directory]`: Change the current directory.
- `pwd`: Display the current working directory.
- `echo [args]`: Print arguments to the terminal.
- `jobs`: List suspended jobs.
- `fg`: Bring the most recent suspended job to the foreground.
- `bg`: Resume a suspended job in the background.
- `exit`: Exit the shell.

## Signal Handling
The Mini Shell handles:
- **SIGINT**: Interrupts the command but keeps the shell running.
- **SIGTSTP**: Suspends running processes and adds them to a job list.

## Command Execution
- **Pipes**: Supports single or multiple pipes to redirect the output of one command to another.
- **External Commands**: The shell also checks a list of external commands stored in a file, and if an external command is detected, it is executed using `execvp()`.

## How It Works
1. The shell waits for user input and parses the command.
2. It checks if the command is a built-in, external, or invalid command.
3. It executes the command directly for built-ins, or forks a new process to execute external commands.
4. The shell handles special conditions such as job control and piping.

## Files Included
- `main.c`: The main code for the Mini Shell.
- `main.h`: The header file containing necessary includes and declarations.
- `external_commands.txt`: A file containing external commands that can be executed by the shell.

## Makefile Integration

The **Makefile** automates the compilation of the dynamic library and the Mini Shell executable:

1. **`dynamic`**: Compiles the `mini_func.c` file as a shared library `libdynamic.so`.
   - This uses the `-fPIC` flag to ensure position-independent code, making it usable as a shared object.

2. **`Mini_Shell`**: Links the main shell code with the dynamic library `libdynamic.so`, producing the executable file `Mini_Shell`.

3. **`run`**: Sets the `LD_LIBRARY_PATH` to include the current directory and runs the `Mini_Shell`.

4. **`clean`**: Cleans up the generated executable and shared library.

## Setup
1. Clone the repository:
   ```bash
   git clone <repository-link>
   cd <project-directory>
   ```
2. Compile the shell using `gcc`:
   ```bash
   gcc main.c -o mini_shell
   ```
3. Run the shell:
   ```bash
   ./mini_shell
   ```

## Usage
- To execute a command, type it after the prompt and press Enter.
- Use built-in commands such as `cd`, `pwd`, `echo`, or execute external commands like `ls`, `cat`, etc.
- The shell supports piping commands like:
  ```bash
  ls | grep main
  ```

## Future Enhancements
- Improve the robustness of signal handling and job management.
- Add more built-in commands.
- Support for more complex piping and redirection functionalities.

## License
This project is licensed under the MIT License.
