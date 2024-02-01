#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>


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
    *data = malloc(infoHeader->biSizeImage);
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

void mergeImages(BITMAPINFOHEADER *infoHeader, BYTE *data1, BYTE *data2, BYTE **outputData, float ratio){
    BYTE *p1, *p2, *outputPixel;
    int x, y, offset;
    int width, height, size;
    height = infoHeader->biHeight;
    width = infoHeader->biWidth;
    size = infoHeader->biSizeImage;
    
    *outputData = malloc(size);
    if(outputData == NULL){
        exit(0);
    }
    
    printf("merging\n");
    
    for(y = 0; y < height; y++) {
        for(x = 0; x < width; x++) {
            offset = (y * width + x) * 3; /*calculate offset for RGB data*/
            p1 = &data1[offset];
            p2 = &data2[offset];
            outputPixel = &(*outputData)[offset];
            /* Merge RGB values based on given ratio*/
            outputPixel[0] = (BYTE) ((p1[0]+p1[1]+p1[2])/3);
            outputPixel[1] = (BYTE) ((1 - ratio) * p1[1] + ratio * p2[1]);
            outputPixel[2] = (BYTE) ((1 - ratio) * p1[2] + ratio * p2[2]);
           
        }
    }
    printf("merge complete\n");
}







int main(int args, char *argv[]){
    char* checkF1;
    char* checkF2;
    char* checkout;
    float ratio;
    BITMAPFILEHEADER fileHeader1, fileHeader2, outputFileHeader;
    BITMAPINFOHEADER infoHeader1, infoHeader2, outputInfoHeader;
    BYTE *data1, *data2, *outputData;

    if(args > 5){
        printf("Too many arguments\n");
        exit(0);
    }
    else if(args < 5){
        printf("Not enough arguments\n");
        exit(0);
    }
    else if(args == 5){
        checkF1 = strchr(argv[1], '.');
        checkF2 = strchr(argv[2], '.');
        checkout = strchr(argv[4], '.');
        ratio = atof(argv[3]);

        if (strcmp(checkF1, ".bmp") != 0){
            printf("Input file 1 is of incorrect type.\n");
            exit(0);
        }
        if (strcmp(checkF2, ".bmp") != 0){
            printf("Input file 2 is of incorrect type.\n");
            exit(0);
        }
        if (strcmp(checkout, ".bmp") != 0){
            printf("output is of incorrect type.\n");
            exit(0);
        }
        if (ratio != 0.5){
            printf("incorrect input for ratio.\n");
            exit(0);
        }

    }
    
    printf("all input is correct\n");


    readFromBMP(argv[1], &fileHeader1, &infoHeader1, &data1);
    readFromBMP(argv[2], &fileHeader2, &infoHeader2, &data2);

    mergeImages(&infoHeader1, data1, data2, &outputData, ratio);
    outputInfoHeader = infoHeader1;
    outputFileHeader = fileHeader1;


    writeToBMP(argv[4], &outputFileHeader, &outputInfoHeader, outputData);
    free(outputData);
    free(data2);
    free(data1);
    printf("images merged successfully\n");
    return 0;
}
