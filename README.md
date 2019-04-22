# forall
This program creates a simple command, forall, that executes a single command multiple times with different arguments. the output of each command will be directed to a file. for example:

$ ./forall wc /etc/passwd /etc/issue
$ cat 1.out
Executing wc /etc/passwd
44 75 2567 /etc/passwd
Finished executing wc /etc/passwd exit code = 0
$ cat 2.out
Executing wc /etc/issue
2 5 26 /etc/issue
Finished executing wc /etc/issue exit code = 0

The first argument is the command to run. Then for each of the rest of the arguments forall will run that command with each argument one at a time. The output (both stdout and stderr) will be redirected to a file that is created for each run (starting at 1). Before running the command, the first line of the file will contain what we are going to run, and after the command finishes we print a line with the exit code.

In the above example, the first argument is the command, wc, followed by two parameters, /etc/passwd and /etc/issue. forall will execute wc /etc/passwd and redirect stdout and stderr to 1.out, when that finishes then forall will execute wc /etc/issue and redirect stdout and stderr to 2.out.
This program also has a signal handler for SIGINT (this happens when you push ^C) to terminate the child rather than the forall program. When the child terminates due to a signal, it is indicated in the output file (see below). SIGQUIT (^\) is used to kill the parent. SIGQUIT should send a SIGINT to any child that is running and then exit the forall program. Here is an example of such a run:

$ ./forall sleep 100 3 200 300
^CSignaling 6220
^\Signaling 6223
Exiting due to quit signal
$ cat 1.out
Executing sleep 100
Stopped executing sleep 100 signal = 2
$ cat 2.out
Executing sleep 3
Finished executing sleep 3 exit code = 0
$ cat 3.out
Executing sleep 200
bcr33d@bcr33d-hp:~/Downloads$ cat 4.out
cat: 4.out: No such file or directory
