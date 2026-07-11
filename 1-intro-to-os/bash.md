# Bash Tutorial

## Get Started with Bash

To check if Bash is installed
```
bash --version
```

To install Bash
```
sudo apt install bash
```

To write a simple script
```
#!/bin/bash
echo "Hello, Bash!"
```
Save this in a file called `hello.sh` and run it with
```
bash hello.sh
```
It prints `Hello, Bash!`.

# Basic Commands

## Common Bash Commands

* `ls` - List directory contents
* `cd` - Change the current directory
* `pwd` - Print the current working directory
* `echo` - Display a line of text
* `cat` - Concatenate and display files
* `cp` - Copy files and directories
* `mv` - Move or rename files
* `rm` - Delete files or folders
* `touch` - Create an empty file or update its time
* `mkdir` - Create a new folder

## `ls` - List Directory Contents

The `ls` command is used to list the contents of a directory e.g. display files, directories, and information about them.

The `ls` command has a variety of options to customize its output:

* `-l` - Long listing format
* `-a` - Include hidden files
* `-h` - Human-readable sizes
* `-t` - Sort by modification time
* `-r` - Reverse order while sorting
* `-R` - List subdirectories recursively
* `-S` - Sort by file size
* `-1` - List one file per line
* `-d` - List directories themselves, not their contents
* `-F` - Append indicator (one of */=@|) to entries

## `cd` - Change Directory

The `cd` command is used to change the current working directory in the terminal.

The `cd` command supports several useful options for navigating directories:

* `cd ..` - Move up one directory level
* `cd ~` - Change to the home directory
* `cd -` - Switch to the previous directory
* `cd /` - Change to the root directory

## `pwd` - Print Working Directory

The `pwd` command shows you the full path of the folder you're currently in.

The `pwd` command supports a few options to customize its output:

* `-L` - Display the logical current working directory
* `-P` - Display the physical current working directory (without symbolic links)

## `echo` - Display Text

The `echo` command is used to show a line of text or a variable's value in the terminal.

The echo command has several options to customize its output:

* `-n` - Don't add a new line at the end
* `-e` - Allow special characters like `\n` for new lines
* `-E` - Don't allow special characters (default)

## `cat` - Concatenate and Display Files

The `cat` command is used to show the content of files in the terminal. 

The `cat` command has options to change how it shows text:

* `-n` - Add numbers to each line
* `-b` - Add numbers only to lines with text
* `-s` - Remove extra empty lines
* `-v` - Show non-printing characters (except for tabs and end of line)

The cat command can be used to concatenate multiple files into one.
```
cat file1.txt file2.txt > combined.txt
```

The `cat` command is often used with piping to send the content of files to other commands.
```
cat my_file.txt | grep "shells"
```

## `cp` - Copy Files and Directories

To copy a file, use `cp source_file destination_file`.

The `cp` command has options to change how it works:

* `-r` - Copy all files and folders inside a directory
* `-i` - Ask before replacing files
* `-u` - Copy only if the source is newer
* `-v` - Verbose mode, show files being copied

## `mv` - Move or Rename Files

To move/rename a file, use `mv source destination`.

The mv command has several options to customize its behavior:

* `-i` - Ask before replacing files
* `-u` - Move only if the source is newer
* `-v` - Verbose mode, show files being moved

## `rm` - Remove Files or Directories

> Be careful, as removed files cannot be easily recovered.

To remove a file, use `rm filename`.

The `rm` command has options to change how it works:

* `-r` - Delete a folder and everything inside it
* `-i` - Ask before deleting each file
* `-f` - Force delete without asking
* `-v` - Verbose mode, show files being removed

## `touch` - Change File Timestamps

The `touch` command is used to change file timestamps or create an empty file if it doesn't exist.

The `touch` command has options to change how it works:

* `-a` - Update only when the file was last read
* `-m` - Update only when the file was last changed
* `-t` - Set the timestamp to a specific time
* `-c` - Do not create any files

## `mkdir` - Make Directories

The `mkdir` command has several options to customize its behavior:

* `-p` - Create parent directories as needed
* `-v` - Show a message for each created directory
* `-m` - Set file mode (permissions)

