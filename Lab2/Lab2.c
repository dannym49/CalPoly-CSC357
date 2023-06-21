
#include<stdio.h>
#include <stdlib.h>
#include<string.h>

typedef struct listelement listelement;

struct listelement{
    listelement *next, *prev;
    char *data;
};

listelement *head = NULL;
int uinput;
char *stringin;
int main();

void push(){
    listelement *newnode= (listelement*)malloc(sizeof(listelement));
    char newstring[100];
    char *new_string = (char*)malloc(strlen(newstring) + 1);
    printf("insert text\n");
    scanf(" %[^\n]", newstring);
    
    strcpy(new_string, newstring);
    newnode ->data=new_string;
    if (head == NULL){
        head= newnode;
        newnode ->next = NULL;
        newnode ->prev = NULL;
    }
    else {
        listelement *current = head;
        while (current->next != NULL) {
            current = current->next;
        }
        newnode->prev = current;
        newnode->next = NULL;
        current->next = newnode;
    }

    printf("done\n\n");
    main();
}

void print(){
    
    listelement *current = head;
    while (current != NULL){
        printf("%s\n", current->data);
        current = current ->next;
    }

    printf("\n");
    main();
}

void reverse(){
    listelement *tail= head;
    while (tail->next != NULL){
        tail = tail->next;
    }

    listelement *current = tail;
    while(current!= NULL){
        listelement *temp = current->next;
        current->next = current->prev;
        current->prev = temp;
        current = current->next;
    }
    head = tail;

    printf("done\n\n");
    main();
}


int delete(){
    int  del;
    printf("delete item #\n");
    scanf("%d", &del);
    listelement *current = head;
    int count = 0;
    while (current != NULL && count <del){
        current = current -> next;
        count ++;
    }

    if (current == NULL){
        printf("no such element\n\n");
        main();
    }

    if (current == head){
        head = current->next;
        if (head != NULL){
            head -> prev = NULL;
        }
    }
    else{
        current->prev->next = current->next;
        if (current->next != NULL){
            current->next->prev = current->prev;
        }
    }

    free(current);
    printf("done\n\n");
    main();
}


void get(){
    int number;
    
    printf("get item #\n");
    scanf("%d", &number);
    listelement *current = head;
    int i = 0;
    while (current != NULL && i < number){
        current = current->next;
        i++;
    }
    if (current == NULL){
        printf("no such element\n\n");
        main();
    }
    else{
        printf(current->data);
    }
    printf("\n\n");
    main();
}
void invalid(){
    printf("invalid entry\n");
   
}


int main() {
    int uinput;

    do {
        printf("1 push string \n2 get item \n3 delete item \n4 reverse list \n5 print list \n6 end program \n\n");
        if (scanf("%d", &uinput) == 1) {
            if (uinput >= 1 && uinput <= 6) {
                if (uinput == 1) {
                    push();
                    
                } else if (uinput == 2) {
                    get();
                } else if (uinput == 3) {
                    delete();
                } else if (uinput == 4) {
                    reverse();
                } else if (uinput == 5) {
                    print();
                } else if (uinput == 6) {
                    exit(0);

                }
            } else {
                printf("invalid entry\n\n");
            }
        } else {
            printf("invalid entry\n\n");
            while (getchar() != '\n') {}
        }
    } while (1);

    return 0;
}












