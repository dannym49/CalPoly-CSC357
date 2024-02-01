#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define MAX_CHILDREN 10
#define MAX_COMMAND_LENGTH 100

typedef struct {
    pid_t pid;
    char command[MAX_COMMAND_LENGTH];
    int pipefd[2];
} ChildProcess;

ChildProcess children[MAX_CHILDREN];
int num_children = 0;

void findFile(char* filename, int searchSubDir) {
    pid_t pid;
    char buffer[MAX_COMMAND_LENGTH];
        ssize_t n;
    int pipefd[2];
    int fileFound = 0;  

    if (pipe(pipefd) == -1) {
        perror("Pipe error\n");
        return;
    }

    pid = fork();

    if (pid == -1) {
        perror("Fork error\n");
        return;
    } 
    else if (pid == 0) {
        
        close(pipefd[0]);

        if (searchSubDir) {
            char* args[] = {"find", ".", "-name", filename, "-execdir", "bash", "-c", "echo $(pwd)", "{}", ";", NULL};
            dup2(pipefd[1], STDOUT_FILENO); 
            execvp("find", args);
        } 
        else {
            char* args[13];
            args[0] = "find";
            args[1] = ".";
            args[2] = "-maxdepth";
            args[3] = "1";
            args[4] = "-name";
            args[5] = filename;
            args[6] = "-execdir";
            args[7] = "bash";
            args[8] = "-c";
            args[9] = "echo $(pwd)";
            args[10] = "{}";
            args[11] = ";";
            args[12] = NULL;
            dup2(pipefd[1], STDOUT_FILENO);  
            execvp("find", args);
        }

        perror("Execvp error\n");
        exit(1);
    } 
    else {
        
        close(pipefd[1]);

        if (num_children < MAX_CHILDREN) {
            children[num_children].pid = pid;
            strncpy(children[num_children].command, filename, MAX_COMMAND_LENGTH);
            children[num_children].pipefd[0] = pipefd[0];
            num_children++;
        } 
        else {
            printf("Max number of children reached\n");
        }

        
        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            fileFound = 1;  
            write(STDOUT_FILENO, buffer, n);
        }

        if (!fileFound) {
            printf("File '%s' not found.\n", filename);
        }
    }
}

void findString(char* searchString, int searchSubDir, char* extension) {
    pid_t pid;
    int pipefd[2];
    int foundMatch = 0;
    char buffer[MAX_COMMAND_LENGTH];
    char* command = NULL;
    char* args[6]; 
    char* filePath;
    ssize_t n;
    if (pipe(pipefd) == -1) {
        perror("Pipe error\n");
        return;
    }

    pid = fork();

    if (pid == -1) {
        perror("Fork error\n");
        return;
    } else if (pid == 0) {
        close(pipefd[0]);

        
        if (extension != NULL && searchSubDir) {
            command = malloc(strlen(extension) + 14);
            strcpy(command, "*.");
            strcat(command, extension);
            args[0] = "grep";
            args[1] = "-rn";
            args[2] = searchString;
            args[3] = ".";
            args[4] = NULL;
            args[5] = NULL;
            args[6] = command;
        } 
        else if (extension != NULL && !searchSubDir) {
            command = malloc(strlen(extension) + 2);
            strcpy(command, "*.");
            strcat(command, extension);
            args[0] = "grep";
            args[1] = "-rn";
            args[2] = searchString;
            args[3] = ".";
            args[4] = "-name";
            args[5] = command;
            args[6] = NULL;
        }
         else if (extension == NULL && !searchSubDir) {
            args[0] = "grep";
            args[1] = "-rn";
            args[2] = searchString;
            args[3] = ".";
            args[4] = NULL;
            args[5] = NULL;
            args[6] = NULL;
        }

        dup2(pipefd[1], STDOUT_FILENO);
        execvp("grep", args);

        perror("Execvp error\n");
        exit(1);
    } 
    else {
        close(pipefd[1]);

        if (num_children < MAX_CHILDREN) {
            children[num_children].pid = pid;
            strncpy(children[num_children].command, searchString, MAX_COMMAND_LENGTH);
            children[num_children].pipefd[0] = pipefd[0];
            num_children++;
        } 
        else {
            printf("Max number of children reached\n");
        }

        
        while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            foundMatch = 1;
            write(STDOUT_FILENO, buffer, n);

            
            filePath = strtok(buffer, ":");
            printf("File path: %s\n", filePath);

           
            findFile(filePath, searchSubDir);
        }
    }

    if (!foundMatch) {
        printf("String '%s' not found in any files.\n", searchString);
    }
}





void listFunc() {
    int i;
    printf("Running child processes:\n");

    for (i = 0; i < num_children; i++) {
        printf("[%d] PID: %d, Command: %s\n", i + 1, children[i].pid, children[i].command);
    }
}

void killFunc(int child_num) {
    if (child_num > 0 && child_num <= num_children) {
        pid_t pid = children[child_num - 1].pid;

        if (kill(pid, SIGKILL) == 0) {
            printf("Child process %d killed.\n", child_num);
        } 
        else {
            perror("kill");
        }
    } 
    else {
        printf("Invalid child number.\n");
    }
}

void cleanupChildren() {
    int i, j;
    for (i = 0; i < num_children; i++) {
        pid_t pid = waitpid(children[i].pid, NULL, WNOHANG);

        if (pid > 0) {
            

            for (j = i; j < num_children - 1; j++) {
                children[j] = children[j + 1];
            }

            num_children--;
            i--;
        }
    }
}

void removeQuotes(char* str) {
    int len = strlen(str);
    
    if (len >= 2 && str[0] == '"' && str[len - 1] == '"') {
        
        char* src = str + 1;
        char* dst = str;
        
        while (*src != '"') {
            *dst = *src;
            src++;
            dst++;
        }
        
        *dst = '\0'; 
    }
}

int main() {
    char command[MAX_COMMAND_LENGTH];
    char originalCommand[MAX_COMMAND_LENGTH];
    char* token;
    char* extension;
    int i;
    char stringSearch[MAX_COMMAND_LENGTH];
    
    while (1) {
        printf("\033[1;34mfindstuff$\033[0m ");  
        fgets(command, MAX_COMMAND_LENGTH, stdin);
        command[strcspn(command, "\n")] = '\0';
        strncpy(originalCommand, command, MAX_COMMAND_LENGTH);
        token = strtok(command, " ");
        
        if (strcmp(command, "find") == 0) {
            
            if (strstr(originalCommand, "\"") != NULL) {
                token = strtok(NULL, " ");
                removeQuotes(token);
                strncpy(stringSearch, token, MAX_COMMAND_LENGTH);
                if (!strstr(originalCommand, " -f") && strstr(originalCommand, " -s")){
                    
                    findString((char*)stringSearch, 1, NULL);
                }
                else if (strstr(originalCommand, " -f") && !strstr(originalCommand, " -s")){
                    printf("search for extension in cd only\n");
                    token = strtok(originalCommand, " ");
                    while (token != NULL) {
                        
                        if (strncmp(token, "-f:", 3) == 0) {
                            extension = token + 3;
                            break;
                        }
                        token = strtok(NULL, " ");
                    }
                    
                    findString((char*)stringSearch, 0, extension);
                    
                }
                else if(strstr(originalCommand, " -f") && strstr(originalCommand, " -s")){
                    printf("search for extension in sub\n");
                    token = strtok(originalCommand, " ");
                    while (token != NULL) {
                        
                        if (strncmp(token, "-f:", 3) == 0) {
                            extension = token + 3;
                            break;
                        }
                        token = strtok(NULL, " ");
                    }
                    findString((char*)stringSearch, 1, extension);
                }
                else {
                    
                    findString((char*)stringSearch, 0, NULL);
                }

            } 
            else {
                
                char* filename = strtok(NULL, " ");
                char* flag = strtok(NULL, " ");

                if (filename != NULL) {
                    int searchSubDir = (flag != NULL && strcmp(flag, "-s") == 0);
                    findFile(filename, searchSubDir);
                } 
                else {
                    printf("Invalid command format. Usage: find <filename> [-s]\n");
                }
            }
            
        } 
        else if (strcmp(token, "list") == 0) {
            listFunc();
        } 
        else if (strcmp(token, "kill") == 0) {
            char* child_num_str = strtok(NULL, " ");

            if (child_num_str != NULL) {
                int child_num = atoi(child_num_str);
                killFunc(child_num);
            } 
            else {
                printf("Invalid command format. Usage: kill <child_num>\n");
            }
        } 
        else if (strcmp(token, "quit") == 0 || strcmp(token, "q") == 0) {
            break;
        } 
        else {
            printf("Invalid command.\n");
        }

        cleanupChildren();
    }

    for (i = 0; i < num_children; i++) {
        close(children[i].pipefd[0]);
        close(children[i].pipefd[1]);
        waitpid(children[i].pid, NULL, 0);
    }

    return 0;
}
