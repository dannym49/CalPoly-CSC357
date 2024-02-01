#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int main(int argc, char *argv[]) {
    int fd[2];
    int array_size, num_processes, i, pid;
    char* args[] = {"EvenOdd", argv[1], argv[2], NULL};
    if (argc != 3) {
        printf("Invalid input\n");
        return 1;
    }

    array_size = atoi(argv[1]);
    num_processes = atoi(argv[2]);
    srand(time(NULL));
    int array[array_size];
    for (i = 0; i < array_size; i++) {
        array[i] = rand();
    }

    pipe(fd);
    if (pipe(fd) == -1) {
        perror("Pipe  ");
        return 1;
    }

    pid = fork();
    if (pid < 0) {
        perror("Fork error");
        return 1;
    }

    if (pid == 0) {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        
        
        execv("./EvenOdd", args);
        perror("Execv error"); 
        exit(EXIT_FAILURE);
    } 
    else {
        close(fd[0]); 
        write(fd[1], array, array_size* sizeof(int));
        close(fd[1]);
        wait(NULL);
    }
    return 0;
}
