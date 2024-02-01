#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PAGESIZE 4096

int initsize = 0;
typedef unsigned char BYTE;
typedef struct chunkhead {
  unsigned int size;
  unsigned int info;
  unsigned char *next, *prev;
} chunkhead;
chunkhead *startofheap = NULL;

unsigned char *mymalloc(unsigned int size) {
  unsigned int bestFit = 0;
  BYTE *ptr = 0;
  chunkhead *largech = 0;
  chunkhead *ch = startofheap;
  chunkhead *prevch = 0;
  size = (size + sizeof(chunkhead)) / PAGESIZE;
  if (size == 0) {
    size = PAGESIZE;
  } else {
    size = ((int)size + 1) * PAGESIZE;
  }
  
  if (initsize == 0) {
    startofheap = (chunkhead *)(uintptr_t)sbrk(0);
    sbrk(size);
    ch = startofheap;
    ch->info = 1;
    ch->prev = 0;
    ch->size = size;
    ch->next = 0;
    initsize = size;
    return (BYTE *)ch + (sizeof(chunkhead));
  }

  if (ch->info == 0 && ch->size == size) {
    ch->info = 1;
    return (BYTE *)ch + (sizeof(chunkhead));
  }
  for (; (ch->next) != 0; ch = (chunkhead *)(ch->next)) {
    if (ch->info == 1 || size > ch->size) {
      prevch = ch;
      continue;
    }
    if (ch->info == 0 && ch->size == size) {
      ch->info = 1;
      return (BYTE *)ch + (sizeof(chunkhead));
    }
    if (ch->info == 0 && ch->size > size) {
      if (bestFit == 0 || bestFit > ch->size) {
        largech = (chunkhead *)((BYTE *)ch + (ch->size) + sizeof(chunkhead) -
                            (size + sizeof(chunkhead)));
        ptr = (BYTE *)ch;
        bestFit = ch->size;
      }
    }
  }
  if (bestFit == 0) {
    sbrk(size);
    initsize += size + sizeof(chunkhead);
    prevch = ch;
    ch = (chunkhead *)((BYTE *)ch + ch->size);
    ch->info = 1;
    ch->size = size;
    ch->next = 0;
    ch->prev = (BYTE *)prevch;
    prevch->next = (BYTE *)ch;
    return (BYTE *)ch + sizeof(chunkhead);
  } 
  else {
    ch = (chunkhead *)ptr;
    prevch = (chunkhead *)(ch->next);
    largech->info = 1;
    ch->info = 0;
    ch->size -= (size) + sizeof(chunkhead);
    largech->prev = (BYTE *)ch;
    largech->size = size;
    ch->next = (BYTE *)largech;
    largech->next = (BYTE *)prevch;
    prevch->prev = (BYTE *)largech;
    return (BYTE *)ch + sizeof(chunkhead);
    
  }
  /*This exit should never run*/
  exit(EXIT_FAILURE);
}

void myfree(unsigned char *address) {
  chunkhead *nextchunk; 
  chunkhead *prevchunk; 
  BYTE *ptr;
  int x;
  chunkhead *ch = (chunkhead *)(address - sizeof(chunkhead));
  ch->info = 0;
  nextchunk = ((chunkhead *)(ch->next));
  prevchunk = ((chunkhead *)(ch->prev));
  if (nextchunk == 0) {
    initsize -= ch->size;
    ptr = (BYTE *)(uintptr_t)sbrk(0);

    brk((BYTE *)ptr - (ch->size - sizeof(chunkhead)));
    if (prevchunk == 0 || initsize == 0) {
      initsize = 0;
      brk(startofheap);
      startofheap = 0;
    }
    if (prevchunk->info == 0) {
      if (prevchunk->prev != 0) {
        prevchunk = (chunkhead *)(prevchunk->prev);
        prevchunk->next = 0;
        x = brk(sbrk(0) - prevchunk->size);
      } else {
        initsize = 0;
        x = brk(startofheap);
        startofheap = 0;
      }
    } else {
      prevchunk->next = ch->next;
    }
  } else if (((chunkhead *)(ch->prev)) == 0 && ((chunkhead *)(ch->next))->info == 0) {
    ch->size += nextchunk->size + sizeof(chunkhead);
    ch->next = nextchunk->next;
    ch->prev = 0;
    if (((chunkhead *)(ch->next)) != 0) {
      ((chunkhead *)(ch->next))->prev = (BYTE *)ch;
    }
  } else if (((chunkhead *)(ch->prev)) == 0 &&
             ((chunkhead *)(ch->next))->info != 0) {
    ch->info = 0;
  } else if (((chunkhead *)(ch->next))->info == 0 &&
             ((chunkhead *)(ch->prev))->info == 0) {
    ch->size += (nextchunk->size) + (prevchunk->size) + sizeof(chunkhead) * 2;
    if (((chunkhead *)(nextchunk->next)) != 0) {
      ((chunkhead *)(nextchunk->next))->prev = (BYTE *)ch;
    }
    if (((chunkhead *)(prevchunk->prev)) != 0) {
      ((chunkhead *)(prevchunk->prev))->next = (BYTE *)ch;
    }
    ch->next = (nextchunk->next);
    ch->prev = (prevchunk->prev);
    prevchunk->size = ch->size;
    prevchunk->next = ch->next;
    prevchunk->prev = ch->prev;
  } else if (nextchunk->info == 0) {
    ch->size += nextchunk->size + sizeof(chunkhead);
    ch->next = nextchunk->next;
    if (((chunkhead *)(ch->next)) != 0) {
      ((chunkhead *)(ch->next))->prev = (BYTE *)ch;
    }
  } else if (prevchunk->info == 0) {
    prevchunk->size += ch->size;
    if (((chunkhead *)(prevchunk->prev)) != 0) {
      ((chunkhead *)(prevchunk->prev))->next = (BYTE *)prevchunk;
    }
    if (((chunkhead *)(nextchunk->next)) != 0) {
      ((chunkhead *)(ch->next))->prev = (BYTE *)prevchunk;
    }
    prevchunk->next = ch->next;
    nextchunk->prev = (BYTE *)prevchunk;
  }
}

void analyze(){
  int no;
  chunkhead* ch;
  printf("\n--------------------------------------------------------------\n");
  if(!startofheap)
  {
  fprintf(stderr, "no heap program break on address: %p\n", (void *)(uintptr_t)sbrk(0));
  return;
  }
  ch = (chunkhead*)startofheap;
  for (no=0; ch; ch = (chunkhead*)ch->next,no++)
  {
  fprintf(stderr,"%d | current addr: %p |", no, (void*)ch);
  fprintf(stderr,"size: %d | ", ch->size);
  fprintf(stderr,"info: %d | ", ch->info);
  fprintf(stderr,"next: %p | ", (void *) ch->next);

  fprintf(stderr,"prev: %p", (void *) ch->prev);
  fprintf(stderr," \n");
  }
  fprintf(stderr, "program break on address: %p\n", (void *)(intptr_t)sbrk(0));

}

int main() {
  int i;
  BYTE*a[100];
  analyze();
  for(i=0;i<100;i++)
  a[i]= mymalloc(1000);
  for(i=0;i<90;i++)
  myfree(a[i]);
  analyze(); 
  myfree(a[95]);
  a[95] = mymalloc(1000);
  analyze();
  for(i=90;i<100;i++)
  myfree(a[i]);
  analyze();
  return(0);
}
