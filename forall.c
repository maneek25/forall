#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <bits/sigaction.h>
#include <stdbool.h>

static int sig = 0;
static int childStatus; 

void sigHandler(int handle) {
    sig = handle;
}

void main(int argc, char *argv[]) {
    int i = 2;
    int fd;
    int pid;
    
    //Sets signal handler
    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = sigHandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("error");
    }
    else if (sigaction(SIGQUIT, &sa, NULL) == -1) {
        perror("perror");
    }

    while (i < argc) {

        char filename[64];
	    sprintf(filename, "%d.out", i-1);
	    fd = open(filename, O_CREAT|O_WRONLY|O_TRUNC, S_IRUSR|S_IWUSR);
		
        pid = fork();
        if (pid < 0) {
	        fprintf(stderr, "fork failed\n");
	        exit(1);
	    }
        else if (pid == 0) {
            dup2(fd, 1);
            write(fd, "Executing ", 10);
            write(fd, argv[1], strlen(argv[1]));
            write(fd, " ", 1);
            write(fd, argv[i], strlen(argv[i]));
            write(fd,"\n", 2);
            execlp(argv[1], argv[1], argv[i], NULL);
            exit(0);
            printf("Should not print\n");
        }
        else {
            char temp[4];
            char temp2[4];

            waitpid(pid, &childStatus, 0);

            //Handles Signals
            if (sig == SIGINT) {      //CTRL C so kill only child
                waitpid(pid, &childStatus, 0);
                printf("Signaling ");
                printf("%d\n", pid);
                sig = 0;
                kill(pid, SIGKILL);
            }
            else if (sig == SIGQUIT) {    //CTRL\ so kill child and parent
                printf("Signaling");
                printf("%d\n", pid);
                printf("Exiting due to quit signal\n");
                kill(pid, SIGKILL);
                exit(0);
            }

            if (WIFEXITED(childStatus)) {       //Command Executed
                write(fd, "Finished executing ", 19);
                write(fd, argv[1], strlen(argv[1]));
                write(fd, " ", 1);
                write(fd, argv[i], strlen(argv[i]));
                write(fd, " exit code = ", 13);
                sprintf(temp, "%d\n", WEXITSTATUS(childStatus));
                write(fd, temp, sizeof(WEXITSTATUS(childStatus)));
            }
            else {          //Command stopped with signal
                write(fd, "Stopped executing ", 19);
                write(fd, argv[1], strlen(argv[1]));
                write(fd, " ", 1);
                write(fd, argv[i], strlen(argv[i]));
                write(fd, " signal = ", 10);
                sprintf(temp2, "%d\n", WTERMSIG(childStatus));
                write(fd, temp2, sizeof(temp2));
            }
        }
        i++;
    }
}


