// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <signal.h>
// #include <sys/mman.h>
// #include <dirent.h>
// #include <sys/stat.h>
// #include <wait.h>
// #include <limits.h>

// #define PATH_MAX 4096

// int fd[2];

// int isDirectory(const char *path) {
//     struct stat statbuff;
//     if (stat(path, &statbuff) != 0)
//         return 0;
//     return S_ISDIR(statbuff.st_mode);
// }

// void findPathSub(DIR *directory, const char *filename, const char *currentPath) {/*searches current dir and sub dirs*/
//     struct dirent *dir;
//     while ((dir = readdir(directory)) != NULL) {
//         if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
//             continue;
//         }

//         char newPath[PATH_MAX];
//         snprintf(newPath, sizeof(newPath), "%s/%s", currentPath, dir->d_name);

//         if (!isDirectory(newPath) && strcmp(dir->d_name, filename) == 0) {
//             printf("File path: %s\n", newPath);
//         }

//         if (isDirectory(newPath)) {
//             DIR *subdirectory = opendir(newPath);
//             if (subdirectory == NULL) {
//                 perror("Error opening subdirectory");
//                 continue;
//             }
//             findPath(subdirectory, filename, newPath);
//             closedir(subdirectory);
//         }
//     }
// }

// void findPath(DIR *directory, const char *filename, const char *currentPath) {/*searches current dir*/
//     struct dirent *dir;
//     int fileFound = 0; 

//     while ((dir = readdir(directory)) != NULL) {
//         if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
//             continue;
//         }

//         char newPath[PATH_MAX];
//         snprintf(newPath, sizeof(newPath), "%s/%s", currentPath, dir->d_name);

//         if (!isDirectory(newPath) && strcmp(dir->d_name, filename) == 0) {
//             printf("File path: %s\n", newPath);
//             fileFound = 1; 
//         }
//     }
//     if (!fileFound) {
//         printf("Nothing found\n");
//     }
// }


// void piperedirect(int i) {
//     dup2(fd[0], STDIN_FILENO);
// }

// int main(int argc, const char *argv[]) {
//     int cpidIndex;
//     int originalInput;
//     char *buffer = malloc(1024);
//     int parent = getpid();
//     int *childpid = mmap(NULL, (10 * sizeof(int)), PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
//     char *inputs = mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
//     int *numofchildren = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
//     int *childDone = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
//     *childDone = 0;

//     signal(SIGUSR1, piperedirect);
//     originalInput = dup(STDIN_FILENO);
//     pipe(fd);
//     fflush(stdout);

//     while (1) {
//         printf("\033[01;34m");
//         printf("\nfindStuff$ ");
//         printf("\033[0m");
//         fflush(stdout);

//         dup2(originalInput, STDIN_FILENO);
//         memset(buffer, 0, 1024);
//         read(STDIN_FILENO, buffer, 1024);
//         fflush(stdout);

//         if (strchr(buffer, 'q') != NULL || strncmp(buffer, "quit", 4) == 0) {
//             int i = 0;
//             while (i++ < 10) {
//                 if (childpid[i] != 0)
//                     kill(childpid[i], SIGKILL);
//             }
//             munmap(childpid, 10 * sizeof(int));
//             munmap(inputs, 1000);
//             munmap(numofchildren, sizeof(int));
//             munmap(childDone, sizeof(int));
//             free(buffer);
//             wait(NULL);
//             return 0;
//         } else if (strncmp(buffer, "list", 4) == 0) {
//             childpid[4] = 1;
//             cpidIndex = 0;
//             while (cpidIndex++ < 10) {
//                 if (childpid[cpidIndex] != 0)
//                     printf("\nProcess %d PID: %d Task: %s\n\n", cpidIndex, childpid[cpidIndex], inputs + cpidIndex * 100);
//             }
//         } else if (strncmp(buffer, "kill", 4) == 0) {
//             char pid[100];
//             memset(pid, 0, sizeof(pid));
//             char *cpidIndex = strchr(buffer, ' ');
//             strncpy(pid, cpidIndex + 1, 8);
//             pid[9] = '\0';

//             int i = 0;
//             while (i++ < 10) {
//                 if (atoi(pid) == childpid[i]) {
//                     (*numofchildren)--;
//                     childpid[i] = 0;
//                     kill(childpid[i], SIGKILL);
//                     waitpid(atoi(pid), 0, 0);
//                 }
//             }
//         } 
//         else {
//             if (*numofchildren < 9) {
//                 (*numofchildren)++;
//                 if (fork() == 0) {
//                     if (!strstr(buffer, " -f") && !strstr(buffer, " -s")) {/*search for file in current dir*/
//                         char *command = strtok(buffer, " ");
//                         char *filename = strtok(NULL, " ");
//                         if (filename != NULL) {
//                             DIR *cwd = opendir(".");
//                             if (cwd == NULL) {
//                                 perror("Error opening current directory\n");
//                                 continue;
//                             }
//                             findPath(cwd, filename, "."); // Pass cwd as the first argument
//                             closedir(cwd);
//                         }
//                     }
//                     else if (!strstr(buffer, " -f") && strstr(buffer, " -s")){/*search for file in current dir and sub dir*/
//                         char *command = strtok(buffer, " ");
//                         char *filename = strtok(NULL, " ");
//                         if (filename != NULL) {
//                             DIR *cwd = opendir(".");
//                             if (cwd == NULL) {
//                                 perror("Error opening current directory\n");
//                                 continue;
//                             }
//                             findPathSub(cwd, filename, "."); 
//                             closedir(cwd);
//                         }
//                     }
//                     exit(0);
//                 }
//             } else {
//                 write(STDOUT_FILENO, "10 children limit reached", 25);
//                 fsync(fd[1]);
//             }
//         }
//     }

//     return 0;
// }
