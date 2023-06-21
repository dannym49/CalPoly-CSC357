#include <stdio.h>
#define PAGESIZE 1024

unsigned char myheap[1048576];

typedef struct chunkhead {
    unsigned int size;
    unsigned int info;
    unsigned char *next, *prev;
} chunkhead;

unsigned char *mymalloc(unsigned int size) {
    chunkhead *cur = (chunkhead *)myheap;

    size = ((size / PAGESIZE) + 1) * PAGESIZE;

    if (cur->info == 0 && cur->size >= size && (cur->info = 1)) {
        unsigned char *p = (unsigned char *)cur + size + sizeof(chunkhead);
        chunkhead *new = (chunkhead *)p;
        new->info = 0;
        new->prev = (unsigned char *)cur;
        new->size = (cur->size) - size - (sizeof(chunkhead));
        cur->size = size;
        cur->next = (unsigned char *)new;
        return (unsigned char *)cur + (sizeof(chunkhead));
    }

    while (cur != NULL) {
        if (cur->info == 1 || cur->size < size) {
            cur = (chunkhead *)(cur->next);
            continue;
        }

        if (cur->info == 0 && cur->size >= size) {
            unsigned char *p = (unsigned char *)cur + size + sizeof(chunkhead);
            chunkhead *new = (chunkhead *)p;
            new->info = 0;
            new->size = (cur->size) - size - (sizeof(chunkhead));
            new->prev = (unsigned char *)cur;
            cur->info = 1;
            cur->size = size;
            cur->next = (unsigned char *)new;
            cur->prev = (unsigned char *)cur->prev;
            return (unsigned char *)cur + sizeof(chunkhead);
        }

        cur = NULL;
    }

    return NULL;
}

void myfree(unsigned char *address) {
  chunkhead *cur = (chunkhead *)(address - sizeof(chunkhead));
  chunkhead *next_chunk = (chunkhead *)(cur->next);
  chunkhead *prev_chunk = (chunkhead *)(cur->prev);
  cur->info = 0;
  
  
  
  if (prev_chunk == NULL && next_chunk->info == 0) {
    cur->size += next_chunk->size + sizeof(chunkhead);
    cur->next = next_chunk->next;
    if (next_chunk->next != NULL) {
      ((chunkhead *)(next_chunk->next))->prev = (unsigned char *)cur;
    }
  }
 
  else if (prev_chunk == NULL && next_chunk->info != 0) {
    cur->info = 0;
  }
  
  else if (next_chunk->info == 0 && prev_chunk->info == 0) {
    cur->size += next_chunk->size + prev_chunk->size + 2 * sizeof(chunkhead);
    if (next_chunk->next != NULL) {
      ((chunkhead *)(next_chunk->next))->prev = (unsigned char *)cur;
    }
    if (prev_chunk->prev != NULL) {
      ((chunkhead *)(prev_chunk->prev))->next = (unsigned char *)cur;
    }
    cur->next = next_chunk->next;
    cur->prev = prev_chunk->prev;
    prev_chunk->size = cur->size;
    prev_chunk->next = cur->next;
    prev_chunk->prev = cur->prev;
  }

  else if (next_chunk->info == 0) {
    cur->size += next_chunk->size + sizeof(chunkhead);
    cur->next = next_chunk->next;
    if (next_chunk->next != NULL) {
      ((chunkhead *)(next_chunk->next))->prev = (unsigned char *)cur;
    }
  }

  else if (prev_chunk->info == 0) {
    cur->size += prev_chunk->size + sizeof(chunkhead);
    if (prev_chunk->prev != NULL) {
      ((chunkhead *)(prev_chunk->prev))->next = (unsigned char *)prev_chunk;
    }
    if (cur->next != NULL) {
      ((chunkhead *)(cur->next))->prev = (unsigned char *)prev_chunk;
    }
    prev_chunk->size = cur->size;
    prev_chunk->next = cur->next;
    prev_chunk->prev = cur->prev;
  }
}

void analyze() {
  chunkhead *cur = (chunkhead *)myheap;
  int i = 1;

  while (cur != 0) {
    printf("\nChunk #%d\nSIZE = %d bytes \n", i, cur->size);
    if (cur->info == 1) {
      printf("Occupied\n");
    } else {
      printf("Free\n");
    }
    printf("Next = %p\nPrev = %p\n", (cur->next), (cur->prev));
    i++;
    cur = (chunkhead *)(cur->next);
  }
}

int main() {
  unsigned char *a, *b, *c;
  chunkhead *cur = (chunkhead *)myheap;
  cur->size = 1048576 - sizeof(chunkhead);
  cur->info = 0;
  cur->prev = 0;
  cur->next = 0;
  a = mymalloc(1000);
  b = mymalloc(1000);
  c = mymalloc(1000);
  myfree(b);
  myfree(c);
  analyze();
  return 0;
}
