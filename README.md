# Simple-Shell-in-C
Simple Linux Shell (Lab1)
1. Objectives
Familiarity with system calls in Unix environment.
Introduction to processes and multi-processing.
Introduction to signal handling in Unix environment.
2. Problem Statement
It is required to implement a Unix shell program. A shell is simply a program that conveniently allows you to run other programs. Read up on your favorite shell to see what it does.

Your shell must support the following commands:

The internal shell command "exit" which terminates the shell
Concepts: shell commands, exiting the shell.
System calls: exit()
A command with no arguments
Example: ls, cp, rm …etc
Details: Your shell must block until the command completes and, if the return code is abnormal, print out a message to that effect.
Concepts: Forking a child process, waiting for it to complete and synchronous execution.
System calls: fork(), execvp(), exit(), waitpid()
A command with arguments
Example: ls –l
Details: Argument 0 is the name of the command.
Concepts: Command-line parameters.
A command, with or without arguments, executed in the background using &.
Example: firefox &
Details: In this case, your shell must execute the command and return immediately, not blocking until the command finishes.
Concepts: Background execution, signals, signal handlers, processes and asynchronous execution.
Requirements: You have to show that the opened process will be nested as a child process to the shell program via opening the task manager found in the operating system like the one shown in figure 1. Additionally you have to write in a log file (basic text file) when a child process is terminated (main application will be interrupted by a SIGCHLD signal). So you have to implement an interrupt handler to handle this interrupt and do the corresponding action to it.
Shell builtin commands
Commands: cd & echo
Details: for the case of:
cd: Cover all the following cases (assume no spaces in path):
cd
cd ~
cd ..
cd absolute_path
cd relative_path_to_current_working_directory
echo: Prints the input after evaluating all expressions (assume input to echo must be within double quotations).
echo "wow" => wow
export x=5
echo "Hello $x" => Hello 5
Expression evaluation
Commands: export
Details: Set values to variables and print variables values. No mathematical operations is needed.
Export Details: Accept input of two forms, either a string without spaces, or a full string inside double quotations.
Example:
export x=-l
ls $x => Will perform ls -l
export y="Hello world"
echo "$y" => Hello world

