#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    char *program1;
    int num_instances;
    char **args;
    int i;
    if (argc != 3) {
        perror("Incorrect number of arguments\n");
        return 1;
    }

    program1 = argv[1];
    num_instances = atoi(argv[2]);

    args = malloc((num_instances + 1) * sizeof(char *));
    if (args == NULL) {
        perror("Memory allocation failed");
        return 1;
    }

    for ( i = 0; i < num_instances; i++) {
        args[i] = malloc(100);
        if (args[i] == NULL) {
            perror("Memory allocation failed");
            return 1;
        }

        sprintf(args[i], "%s %d %d", program1, i, num_instances);
    }

    args[num_instances] = NULL;

    for ( i = 0; i < num_instances; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Fork failed");
            return 1;
        } else if (pid == 0) {
            
            execv(program1, args);
            perror("Execv failed");
            return 1;
        }
    }

    
    for ( i = 0; i < num_instances; i++) {
        wait(NULL);
    }

    for ( i = 0; i <= num_instances; i++) {
        free(args[i]);
    }
    free(args);

    return 0;
}
