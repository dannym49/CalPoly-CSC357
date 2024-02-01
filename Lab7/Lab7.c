#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char BYTE;

typedef struct mypipe
{
BYTE* pipebuffer;
int buffersize;
int start_occupied;
int end_occupied;
}mypipe;

void init_pipe(mypipe* pipe, int size){
    pipe->pipebuffer = (BYTE*)malloc(sizeof(BYTE) * size);
    pipe->buffersize = size;
    pipe->start_occupied = 0;
    pipe->end_occupied = 0;
}

int mywrite(mypipe* pipe, BYTE* buffer, int size) {
    int bytes_to_write;
    int available_space = (pipe->start_occupied - pipe->end_occupied - 1 + pipe->buffersize) % pipe->buffersize;

    if (available_space == 0) {
        return 0;  
    }
    bytes_to_write = (size < available_space) ? size : available_space;
    if (pipe->end_occupied + bytes_to_write > pipe->buffersize) {
        int partial_size = pipe->buffersize - pipe->end_occupied;
        memcpy(pipe->pipebuffer + pipe->end_occupied, buffer, partial_size);
        memcpy(pipe->pipebuffer, buffer + partial_size, bytes_to_write - partial_size);
    } 
    else {
        memcpy(pipe->pipebuffer + pipe->end_occupied, buffer, bytes_to_write);
    }

    pipe->end_occupied = (pipe->end_occupied + bytes_to_write) % pipe->buffersize;

    return bytes_to_write;
}

int myread(mypipe* pipe, BYTE* buffer, int size) {
    int byteRead;
    int spaceOccupied = (pipe->end_occupied - pipe->start_occupied + pipe->buffersize) % pipe->buffersize;
    if (spaceOccupied == 0){
        return 0;  
    }
    byteRead = (size < spaceOccupied) ? size : spaceOccupied;
    if (pipe->start_occupied + byteRead > pipe->buffersize) {
        int partial_size = pipe->buffersize - pipe->start_occupied;
        memcpy(buffer, pipe->pipebuffer + pipe->start_occupied, partial_size);
        memcpy(buffer + partial_size, pipe->pipebuffer, byteRead - partial_size);
    } 
    else {
        memcpy(buffer, pipe->pipebuffer + pipe->start_occupied, byteRead);
    }
    pipe->start_occupied = (pipe->start_occupied + byteRead) % pipe->buffersize;
    return byteRead;
}

int main(){/*
    char text[100];
    mypipe pipeA;
    init_pipe(&pipeA, 32);
    mywrite(&pipeA, "hello world", 12);
    mywrite(&pipeA, "it's a nice day", 16);
    myread(&pipeA, text, 12);
    printf("%s\n", text);
    myread(&pipeA, text, 16);
    printf("%s\n", text);
    mywrite(&pipeA, "and now we test the carryover", 30);
    myread(&pipeA, text, 30);
    printf("%s\n", text);*/
}
