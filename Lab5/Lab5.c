#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <libgen.h>

void sighandler(int n)
{
    printf("\n no \n");
}

int main()
{
    int i;
    struct dirent *entry;
    DIR *dir = NULL;
    time_t T = time(NULL);
    struct tm tm = *localtime(&T);
    printf("PID before fork: %d", getpid());
    fflush(0);
    for (i = 1; i< 65; i++){
                signal(i, sighandler);
    }
    
    while(1)
    {
        if (fork() == 0)
        {
            while(1)
            {
            printf("Childs PID: %d\n", getpid());
            printf("Current time: %d:%d\n", tm.tm_hour, tm.tm_min);
            dir = opendir(".");
            while((entry = readdir(dir))!= NULL){
                printf("%s \n", entry->d_name);
            }
            printf("\n");
            sleep(10);
            }
            return 0;
        }
        else
        {
            printf("\nParent PID: %d\n", getpid());
            wait(NULL);
        }

    }
    return 0;
}
