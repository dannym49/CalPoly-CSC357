
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <string.h>

void sighandler()
{
    printf("Child cannot be killed.\n");
}

int main()
{
    int i;
    char userInput[50];
    DIR *d;
    struct stat st;
    int *childpid = mmap(NULL, 4, PROT_READ | PROT_WRITE, 0x20 | MAP_SHARED, -1, 0);
    int *childrunning = mmap(NULL, 4, PROT_READ | PROT_WRITE, 0x20 | MAP_SHARED, -1, 0);
    char *buffer = mmap(NULL, 2000, PROT_READ | PROT_WRITE, 0x20 | MAP_SHARED, -1, 0);
    buffer = getcwd(NULL, 100);
    for (i = 1; i< 65; i++){
                signal(i, sighandler);
    }
    
    
    *childrunning = 1;
    
    fflush(NULL);
    while(1)
    {

    if (fork() == 0)
    {
        while(1)
        {
            chdir(buffer);
            d = opendir(buffer);
            *childpid = getpid();            
            printf("\033[01;34m");/*sets console color*/
            printf("\nStat Prog: %s", buffer);
            printf("\033[0m");
            printf("$");

            scanf("%s", userInput);
            *childrunning += 1;
            if (strcmp(userInput, "..") == 0)
            {
                chdir("..");
                strcpy(buffer, getcwd(NULL, 100));
                fflush(0);
            }
            else if (userInput[0] == '/')
            {
                char * dname = userInput + 1;
                if (stat(dname, &st) != 0)
                {
                    printf("\nFolder does not exist.\n");
                    break;
                } 
                strcat(buffer, userInput);
                *buffer = *getcwd(NULL, 100);
                
            }
            else if (strcmp(userInput, "q") == 0)
            {
                *childrunning = 0;
                sleep(10);
                
            }
            else if (strcmp(userInput, "list") == 0)
            {
                struct dirent *dir;
                printf("\n");
                while ((dir = readdir(d)) != NULL)
                {
                    printf("%s ", (dir->d_name));
                    printf("\n");
                }
                printf("\n");
                closedir(d);
            }
            else if ((stat(userInput, &st) != 0))
            {
                printf("\nFile does not exist\n");
            }
            else
            {
                stat(userInput, &st);
                printf("File: %s", userInput);
                printf("\nSize: %ld", (long int)st.st_size);
                printf("\tBlocks: %ld", (long)st.st_blocks);
                printf("\tIO Block: %ld", (long int)st.st_blksize);
                printf("\nDevice: %lu", st.st_dev);
                printf("\tInode: %lu", (unsigned long)st.st_ino);
                printf("\tLinks: %lu", (unsigned long)st.st_nlink);
                printf("\nAccess: %d", st.st_mode);
                printf("\tUid:( %d/danny)", st.st_uid);
                printf("\tGid: ( %d/danny)", st.st_gid);
                printf("\nDevice ID: %lu", (unsigned long)st.st_rdev);
                printf("\nAccess: %s", ctime(&st.st_atime));
                printf("Modified: %s", ctime(&st.st_mtime));
            }
        }
        return 0;
    }
    else
    {
        int g;
        wait(&g);
        sleep(10);
        if (*childrunning == 0)
        {
            
            kill(*childpid, SIGKILL);
            if (waitpid(*childpid, &g, WNOHANG | WUNTRACED) != -1)
            {
                kill(*childpid, SIGKILL);
            }  
            munmap(&childpid, 4);
            munmap(&buffer, 2000);
            munmap(&childrunning, 4);
            wait(0);
            kill(getpid(), SIGKILL);
            return 0;
        }
        chdir(buffer);
        }
    }      
    return 0;
}