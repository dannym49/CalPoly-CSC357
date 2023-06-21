#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>
#include<sys/mman.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/wait.h>
#include<time.h>

typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned int LONG;

typedef struct BITMAPFILEHEADER BITMAPFILEHEADER;
typedef struct BITMAPINFOHEADER BITMAPINFOHEADER;
typedef unsigned char BYTE;

#pragma pack(push,1)

struct BITMAPFILEHEADER
{
WORD bfType; 
DWORD bfSize;
WORD bfReserved1; 
WORD bfReserved2; 
DWORD bfOffBits; 
};


struct BITMAPINFOHEADER
{
DWORD biSize; /* specifies the number of bytes required by the struct*/
LONG biWidth; /* specifies width in pixels*/
LONG biHeight; /* species height in pixels*/
WORD biPlanes; /* specifies the number of color planes, must be 1*/
WORD biBitCount; /* specifies the number of bit per pixel*/
DWORD biCompression;/* spcifies the type of compression*/
DWORD biSizeImage; /* size of image in bytes*/
LONG biXPelsPerMeter; /* number of pixels per meter in x axis*/
LONG biYPelsPerMeter; /* number of pixels per meter in y axis*/
DWORD biClrUsed; /* number of colors used by th ebitmap*/
DWORD biClrImportant; /* number of colors that are important*/
};

#pragma pack(pop)



void readFromBMP(char *fileName, BITMAPFILEHEADER *fileHeader, BITMAPINFOHEADER *infoHeader, BYTE **data){
    FILE *file = fopen(fileName, "rb");
    if (file == NULL){
        printf("File doesnt exist\n");
        exit(0);
    }
    fread(fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fread(infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    *data = mmap(NULL, infoHeader->biSizeImage, PROT_READ | PROT_WRITE, MAP_SHARED | 0x20, -1, 0);
    if(*data == NULL){
        printf("Image data error\n");
        exit(0);
    }
    fseek(file, fileHeader->bfOffBits, SEEK_SET);
    fread(*data, infoHeader->biSizeImage,1, file);
    printf("read done\n");
    fclose(file);

}

void writeToBMP(char *fileName, BITMAPFILEHEADER *fileHeader, BITMAPINFOHEADER *infoHeader, BYTE *data){
    FILE *file = fopen(fileName, "wb");
    fwrite(fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
    fwrite(infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
    fwrite(data, infoHeader->biSizeImage, 1, file);
    printf("write to file complete\n");
    fclose(file);
}

void increaseBrightness(BYTE *data, int size, float brightness){
    int i;
    int bright; 
    for (i = 0; i<size; i++){
        bright = data[i] + (brightness * 255);
        if (bright > 255){
            data[i] = 255;
        }
        else{
            data[i] = (BYTE) bright;
        }
    }
    printf("brightness done\n");
}

int main(int args, char *argv[]){
    clock_t start, end;
    double timediff;
    float brightness = atof(argv[2]);
    int parallel = atoi(argv[3]);
    BITMAPFILEHEADER fileHeader1, outputFileHeader;
    BITMAPINFOHEADER infoHeader1, outputInfoHeader;
    BYTE *data1;
    readFromBMP(argv[1], &fileHeader1, &infoHeader1, &data1);
    

    if (parallel == 0){
        increaseBrightness(data1, infoHeader1.biSizeImage, brightness);
        
    }
    else if (parallel == 1)
    {
        int halfsize = ((infoHeader1.biSizeImage / sizeof(BYTE)) / 2);
        int pid = fork();

        start = clock();

        if (pid == -1){
            printf("Fork error\n");
            exit(EXIT_FAILURE);
        }
        else if (pid == 0)
        {   
            increaseBrightness(data1, halfsize, brightness);
            exit(0);
        }
        
        else if (pid > 0){
            wait(NULL);
            increaseBrightness(data1 + halfsize, halfsize, brightness);
            
        }
        end = clock();
        timediff = (double)(end-start)/CLOCKS_PER_SEC;
        printf("%f \n", timediff);

    }
    
    outputInfoHeader = infoHeader1;
    outputFileHeader = fileHeader1;
    writeToBMP(argv[4], &outputFileHeader, &outputInfoHeader, data1);
    munmap(data1, infoHeader1.biSizeImage);
    printf("images merged successfully\n");
    return 0;
}
