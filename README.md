# Basic Unix Shell in C
### What is a Shell?
A shell is special user program which provide an interface to user to use operating system services. Shell accepts human readable commands from user and convert them into something which kernel can understand. It is a command language interpreter that execute commands read from input devices such as keyboards or from files. The shell gets started when the user logs in or start the terminal.

In general, operating system shells use either a **command-line interface (CLI)** or **graphical user interface (GUI)**, depending on a computer's role and particular operation. It is named a shell because it is the outermost layer around the operating system kernel.

### How does the shell work?
The shell works by running an infinite loop and performing the following functions inside loop:

1. **Read** - Read the command from standard input.
2. **Parse** - Separate the command string into program and arguments and look for pipes, I/O Redirection etc.
3. **Execute** - Run the parsed commands.
     - Create child process.
     - Execute child process.
     - Wait for child process to finish.

### Commands Handled by Shell Program
- **Internal Commands :** `cd` `echo` `history` `pwd` `exit`
- **External Commands :** `ls` `cat` `date` `mkdir` `rm`

### Assumptions
- User only enters the commands handled by the shell else the shell will give an error message to user.
- Commands do not have any piping or I/O redirection.
- Commands must be on a single line.
- Arguments must be separated by whitespace.

### Errors Handled
- Wrong Input Command.
- Memory not available.
- No Arguments Passed

## References
1. [Stephen Brennan's Write a Shell in C Tutorial](https://brennan.io/2015/01/16/write-a-shell-in-c/)
2. [Making your own Linux Shell in C](https://www.geeksforgeeks.org/making-linux-shell-c/)
3. [Introduction to Linux Shell and Shell Scripting](https://www.geeksforgeeks.org/introduction-linux-shell-shell-scripting/)
