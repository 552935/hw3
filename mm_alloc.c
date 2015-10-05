/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
/* Your final implementation should comment out this macro. */
void *base = NULL;
struct bar {
  size_t size;
  bool free;
  struct bar* next;
  struct bar* prev;
};
#define BAR_SIZE sizeof(struct bar)
struct bar* bar2 (struct bar *prev, size_t size){
	
	struct bar *newBar;
	newBar = sbrk(0); 
	sbrk(size+BAR_SIZE); 
	if (prev){	
		prev->next=newBar;
		newBar->prev=prev;
	}
	newBar->size = size;
	newBar->next = NULL;
	newBar->free = false;

	return newBar;	
}
struct block* splitBlock(struct bar* bar1, size_t size){
	
	if (bar1->size>=1+size+BAR_SIZE){  
		struct bar * mbar = (struct bar *)((char*)bar1+BAR_SIZE+size); 
		mbar->size = bar1->size-(size+BAR_SIZE); 
		mbar->free=true;
		mbar->next=bar1->next;	
		mbar->prev=bar1;
		if (bar1->next){
			bar1->next->prev=mbar;
		}
		bar1->size=size;
		bar1->next = mbar;
	}
	return bar1;	
}

struct bar* Free(struct bar *prev, size_t size){	
	struct bar *thisbar = base;	

	while (true){	
		if (!thisbar){	
			return bar2(prev ,size);
		}
		if (thisbar&&thisbar->free && (thisbar->size)>=size){	
			thisbar=splitBlock(thisbar,size);
			thisbar->free=false;
			return thisbar;	
		}
		prev = thisbar; 
		thisbar = thisbar->next;	
	}
	return 0;	
}
void merge(struct bar *thisbar){	
	if (thisbar->next){	
		if (thisbar->next->free){	
			thisbar->size+=thisbar->next->size+BAR_SIZE;	
			if (thisbar->next->next){	
				thisbar->next->next->prev=thisbar;
				thisbar->next=thisbar->next->next;
			}
		}
	}
	if (thisbar->prev){
		if (thisbar->prev->free){	
			merge(thisbar->prev);
		}
	}
}


void* mm_malloc(size_t size)	
{
	struct bar* newBar;
	if (base){	
		struct bar* finalbar=base;
		newBar=Free(finalbar, size);
	}else{	
                newBar=bar2(NULL,size);
		base=newBar;
	}
  return (void*) ((long) newBar+BAR_SIZE);

}



void* mm_realloc(void* ptr, size_t size)
{
void *newBarPtr = mm_malloc(size);	
	struct bar* prevbar = (struct bar*) ptr - 1;	
  memcpy(newBarPtr, ptr, prevbar->size);	 
  mm_free(ptr);	
  return newBarPtr;	


}

void mm_free(void* ptr)	
{
	struct bar* thisbar = (struct bar*) ptr - 1;	
	thisbar->free=true;	
	merge(thisbar);	
}

