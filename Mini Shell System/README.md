# Mini Shell System
This program simulates a mini shell system in the C library. 
# Usage
Simply clone the repository or copy all files into the same directory. Run the make command on your desired command line interface (or gcc command), and simply run the executable. 
# What I Learned
- The exec command is used to replace a process image while the fork command creates a new child process. 
- We can run system commands in our shell program without replacing the process by first creating a child process and then running the commands using exec. 
