# Linux Mini Shell

A custom Linux shell implemented in C to understand how Linux shells such as Bash internally handle command execution, process creation, pipes, signals, and job control.

The shell continuously displays a prompt, accepts user commands, identifies whether the command is built-in or external, and executes it accordingly.

## Features

### Custom Prompt

* Supports dynamic prompt modification using:

  ```
  PS1=new_prompt
  ```

### Built-in Commands

Implemented built-in commands:

* cd
* pwd
* exit
* echo $$
* echo $?
* echo $SHELL
* jobs
* fg
* bg

### External Command Execution

* Supports execution of 153 Linux external commands
* Uses:

  * fork()
  * execvp()
  * waitpid()

Examples:

```
ls
cat file.txt
grep hello file.txt
mkdir test
rm file.txt
```

### Pipe Support

Supports single and multiple pipelines.

Examples:

```
ls | grep txt
cat file.txt | grep hello | wc
```

Uses:

* pipe()
* dup2()
* fork()
* execvp()

### Signal Handling

Handles:

* Ctrl + C (SIGINT)
* Ctrl + Z (SIGTSTP)
* SIGCHLD

The shell remains active while child processes respond appropriately.

### Job Control

Supports:

* jobs
* fg
* bg

Stopped jobs are maintained using a linked list and can be resumed in foreground or background execution.

## Project Architecture

Input Command
→ Command Parsing
→ Built-in / External Detection
→ Process Creation
→ Command Execution
→ Job Control & Signal Handling

## Data Structures Used

### Linked List

Used for:

* Job management
* Storing stopped processes
* Maintaining PID and command information

### Arrays

Used for:

* Built-in command table
* External command table
* Argument vectors (argv)

## System Calls Used

* fork()
* execvp()
* waitpid()
* pipe()
* dup2()
* chdir()
* getcwd()
* kill()
* signal()

## Concepts Learned

* Linux Process Creation
* Parent-Child Process Relationship
* fork-exec Mechanism
* Command Parsing
* Process Synchronization
* Pipes and IPC
* Signal Handling
* Job Control
* File Descriptors
* Shell Internals

## Challenges Faced

* Understanding fork() and execvp() interaction
* Implementing pipe-based command execution
* Managing foreground and background jobs
* Handling Ctrl+C and Ctrl+Z correctly
* Maintaining stopped processes using linked lists

## Outcome

Successfully developed a Linux Mini Shell capable of executing built-in and external commands, supporting pipes, handling signals, and providing basic job control while demonstrating core Linux operating system concepts and process management.
