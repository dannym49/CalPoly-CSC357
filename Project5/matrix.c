#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <semaphore.h>
#define MATRIX_DIMENSION_XY 10

void set_matrix_elem(float *M,int x,int y,float f)
{
    M[x + y*MATRIX_DIMENSION_XY] = f;
}

int quadratic_matrix_compare(float *A,float *B)
{
    int a, b;
    for(a = 0;a<MATRIX_DIMENSION_XY;a++)
        for(b = 0;b<MATRIX_DIMENSION_XY;b++)
        if(A[a +b * MATRIX_DIMENSION_XY]!=B[a +b * MATRIX_DIMENSION_XY]) 
            return 0;
    
    return 1;
}

void quadratic_matrix_print(float *C)
{
    int a,b;
    printf("\n");
    for( a = 0;a<MATRIX_DIMENSION_XY;a++)
        {
        printf("\n");
        for( b = 0;b<MATRIX_DIMENSION_XY;b++)
            printf("%.2f,",C[a + b* MATRIX_DIMENSION_XY]);
        }
    printf("\n");
}

void quadratic_matrix_multiplication(float *A,float *B,float *C)
{
    int a,b,c;
    for( a = 0;a<MATRIX_DIMENSION_XY;a++)
        for( b = 0;b<MATRIX_DIMENSION_XY;b++)
            C[a + b*MATRIX_DIMENSION_XY] = 0.0;

    for( a = 0;a<MATRIX_DIMENSION_XY;a++) 
        for( b = 0;b<MATRIX_DIMENSION_XY;b++) 
            for( c = 0;c<MATRIX_DIMENSION_XY;c++) 
                {
                    C[a + b*MATRIX_DIMENSION_XY] += A[c + b*MATRIX_DIMENSION_XY] * B[a + c*MATRIX_DIMENSION_XY]; 
                }
}

void synch(int par_id, int par_count, int* ready)
{
    if (par_id == 0)
    {
        sem_post((sem_t *)ready);
        sem_wait((sem_t *)ready);
    }
    else
    {
        sem_post((sem_t *)ready);
        sem_wait((sem_t *)ready);
    }
}



void quadratic_matrix_multiplication_parallel(int par_id, int par_count, float *A, float *B, float *C) {
    int rows_per_process = MATRIX_DIMENSION_XY / par_count;
    int start_row = par_id * rows_per_process;
    int end_row = start_row + rows_per_process;
    int a,b,c;
    for ( a = start_row; a < end_row; a++) {
        for ( b = 0; b < MATRIX_DIMENSION_XY; b++) {
            for ( c = 0; c < MATRIX_DIMENSION_XY; c++) {
                C[a + b * MATRIX_DIMENSION_XY] += A[c + b * MATRIX_DIMENSION_XY] * B[a + c * MATRIX_DIMENSION_XY];
            }
        }
    }
}


int main(int argc, char *argv[])
{
    int i,j;
    int par_id = 0; 
    int par_count = 1;
    float *A,*B,*C; 
    int *ready;
    int fd[4];
    float M[MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY];
    if(argc!=3){printf("no shared\n");}
    else
        {
        par_id= atoi(argv[1]);
        par_count= atoi(argv[2]);
    
        }
    if(par_count==1){
        printf("only one process\n");
        }

   
    if(par_id==0)
        {
        
        fd[0] = shm_open("matrixA", O_CREAT | O_RDWR, 0666);
        fd[1] = shm_open("matrixB", O_CREAT | O_RDWR, 0666);
        fd[2] = shm_open("matrixC", O_CREAT | O_RDWR, 0666);
        fd[3] = shm_open("synchobject", O_CREAT | O_RDWR, 0666);

        ftruncate(fd[0], sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY);
        ftruncate(fd[1], sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY);
        ftruncate(fd[2], sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY);
        ftruncate(fd[3], sizeof(int));

        A = (float *)mmap(NULL, sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        B = (float *)mmap(NULL, sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
        C = (float *)mmap(NULL, sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
        ready = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);
        }
    else
        {
        
        fd[0] = shm_open("matrixA", O_RDWR, 0666);
        fd[1] = shm_open("matrixB", O_RDWR, 0666);
        fd[2] = shm_open("matrixC", O_RDWR, 0666);
        fd[3] = shm_open("synchobject", O_RDWR, 0666);

        A = (float *)mmap(NULL, sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[0], 0);
        B = (float *)mmap(NULL, sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[1], 0);
        C = (float *)mmap(NULL, sizeof(float) * MATRIX_DIMENSION_XY * MATRIX_DIMENSION_XY, PROT_READ | PROT_WRITE, MAP_SHARED, fd[2], 0);
        ready = (int *)mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd[3], 0);

        sleep(2); 
        }


    synch(par_id,par_count,ready);

    if(par_id==0)
        {
        
        for ( i = 0; i < MATRIX_DIMENSION_XY; i++)
        {
            for ( j = 0; j < MATRIX_DIMENSION_XY; j++)
            {
                set_matrix_elem(A, i, j, i + j);
                set_matrix_elem(B, i, j, i * j);
            }
        }
        }

    synch(par_id,par_count,ready);

    
    quadratic_matrix_multiplication_parallel(par_id, par_count, A, B, C);

        
    synch(par_id,par_count,ready);

    if(par_id==0)
        quadratic_matrix_print(C);
    synch(par_id, par_count, ready);

    
    
    quadratic_matrix_multiplication(A, B, M);
    if (quadratic_matrix_compare(C, M))
        printf("full points!\n");
    else
        printf("buuug!\n");


    close(fd[0]);
    close(fd[1]);
    close(fd[2]);
    close(fd[3]);
    shm_unlink("matrixA");
    shm_unlink("matrixB");
    shm_unlink("matrixC");
    shm_unlink("synchobject");

    return 0;    
}
