#include<stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]){
    int i;
    int spaces;
    int height;
    int j= 0;
    int stem = 3;
    FILE* outfile;
    
    if (argc == 1){
        
        printf("Enter a number between 1 and 15: ");
        scanf("%d", &height);
        printf("\n");
        if (height > 15 || height < 1){
            printf("You entered an incorrect input. \n");
            return 0;
        }
    }
    else if (argc == 2){
            height = atoi(argv[1]);
        }

    else if (argc == 3 && strcmp(argv[1], "<")== 0){
        //gets height from text 
        scanf("%d", &height);
        if (height > 15 || height < 1){
            printf("You entered an incorrect input. \n");
            return 0;
        }
    }
    else if (argc == 3){
        height = atoi(argv[1]);
        outfile = fopen(argv[2], "w");
        
        for(i = 1; i <= height-3; i++, j = 0){
        for(spaces = 1; spaces <= height - i; spaces++){
            printf(" "); //prints the spaces in each row
            fprintf(outfile, " ");
        }
        while (j != 2 * i - 1){
            printf("*");
            fprintf(outfile, "*");
            j = j + 1;
        }
        printf("\n"); //moves onto the next row
        fprintf(outfile, "\n");
        }
  
    
        for(i = 1; i<=stem; i++){
            for(j=1; j<=((height*2)/2)-1; j++){
                printf(" ");
                fprintf(outfile, " ");
            }
            printf("*\n");
            fprintf(outfile, "*\n");
        }
        return 0;
    }
    
    
    for(i = 1; i <= height-3; i++, j = 0){
        for(spaces = 1; spaces <= height - i; spaces++){
            printf(" "); //prints the spaces in each row
        }
        while (j != 2 * i - 1){
            printf("*");
            j = j + 1;
        }
        printf("\n"); //moves onto the next row
    }
  
    
    for(i = 1; i<=stem; i++){
        for(j=1; j<=((height*2)/2)-1; j++){
            printf(" ");
        }
        printf("*\n");
    }
    return 0;
}
