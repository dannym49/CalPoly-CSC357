#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

void swap(int array[], int i, int j) {
    int temp = array[i];
    array[i] = array[j];
    array[j] = temp;
}

void oddEvenSort(int array[], int size, int num_processes) {
    int i, j, k;
    pid_t pid;
    int sorted = 0;

    for (i = 0; i < size && !sorted; i++) {
        sorted = 1;

        if (i % 2 == 0) {
            for (j = 1; j < size - 1; j += 2) {
                for (k = 0; k < num_processes; k++) {
                    pid = fork();

                    if (pid == 0) {
                        if (array[j] > array[j + 1]) {
                            swap(array, j, j + 1);
                        }

                        exit(0);
                    } else if (pid < 0) {
                        printf("Fork failed.\n");
                        exit(1);
                    }
                }

                for (k = 0; k < num_processes; k++) {
                    wait(NULL);
                }
            }
        } 
        else {
            for (j = 0; j < size - 1; j += 2) {
                for (k = 0; k < num_processes; k++) {
                    pid = fork();

                    if (pid == 0) {
                        if (array[j] > array[j + 1]) {
                            swap(array, j, j + 1);
                        }

                        exit(0);
                    } else if (pid < 0) {
                        printf("Fork failed.\n");
                        exit(1);
                    }
                }

                for (k = 0; k < num_processes; k++) {
                    wait(NULL);
                }
            }
        }

        for (j = 0; j < size - 1; j++) {
            if (array[j] > array[j + 1]) {
                sorted = 0;
                break;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    double elapsed_time;
    clock_t start_time, end_time;
    int num_processes, size, num, i, shmid;
    int *sharedarray;

    if (argc == 2){
        start_time = clock();
        num_processes = atoi(argv[1]);
        /*standalone*/
        size = 0;
        

        while (scanf("%d", &num) == 1) {
            size++;
        }

        int array[size];

        rewind(stdin);

        i = 0;
        while (scanf("%d", &num) == 1) {
            array[i] = num;
            i++;
        }

        shmid = shmget(IPC_PRIVATE, sizeof(int) * size, IPC_CREAT | 0666);
        if (shmid == -1) {
            perror("shmget");
            exit(1);
        }

        sharedarray = (int *)shmat(shmid, NULL, 0);
        if (sharedarray == (int *)-1) {
            perror("shmat");
            exit(1);
        }

        for (i = 0; i < size; i++) {
            sharedarray[i] = array[i];
        }

        printf("Initial arrayay: [");
        for (i = 0; i < size; i++) {
            printf("%d", sharedarray[i]);
            if (i != size - 1) {
                printf(", ");
            }
        }
        printf("]\n");

        oddEvenSort(sharedarray, size, num_processes);

        printf("Sorted arrayay: [");
        for (i = 0; i < size; i++) {
            printf("%d", sharedarray[i]);
            if (i != size - 1) {
                printf(", ");
            }
        }
        printf("]\n");
        printf("Number of processes: %d \n", num_processes);
        end_time = clock();
        elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
        printf("Time to sort: %.6f ms\n", elapsed_time);

        if (shmdt(sharedarray) == -1) {
            perror("shmdt");
            exit(1);
        }

        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
        }
        return 0;
    }
    else {
        /*randomizer*/
        if (argc < 3) {
            printf("Usage: EvenOdd <arrayay_size> <num_processes>\n");
            return 1;
        }
        start_time = clock();
        size = atoi(argv[1]);
        num_processes = atoi(argv[2]);
        int array[size];

        read(STDIN_FILENO, array, sizeof(int) * size);
        
        
        shmid = shmget(IPC_PRIVATE, sizeof(int) * size, IPC_CREAT | 0666);
        if (shmid == -1) {
            perror("shmget");
            exit(1);
        }

        sharedarray = (int *)shmat(shmid, NULL, 0);
        if (sharedarray == (int *)-1) {
            perror("shmat");
            exit(1);
        }

        for (i = 0; i < size; i++) {
            sharedarray[i] = array[i];
        }

        printf("Initial arrayay: [");
        for (i = 0; i < size; i++) {
            printf("%d", sharedarray[i]);
            if (i != size - 1) {
                printf(", ");
            }
        }
        printf("]\n");

        oddEvenSort(sharedarray, size, num_processes);

        printf("Sorted arrayay: [");
        for (i = 0; i < size; i++) {
            printf("%d", sharedarray[i]);
            if (i != size - 1) {
                printf(", ");
            }
        }
        printf("]\n");
        printf("Number of processes: %d \n", num_processes);
        end_time = clock();
        elapsed_time = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000;
        printf("Time to sort: %.6f ms\n", elapsed_time);

        if (shmdt(sharedarray) == -1) {
            perror("shmdt");
            exit(1);
        }

        
        if (shmctl(shmid, IPC_RMID, NULL) == -1) {
            perror("shmctl");
            exit(1);
        }

        return 0;
    }
    
}
