#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

int hinit(size_t size);
void *halloc(size_t size);
void hfree(void *ptr);

#endif


feat(heap): add heap interface

- add hinit, halloc, hfree function definitions
  - hinit: initialize heap
  - halloc: allocate memory from heap
  - hfree: free memory back to heap
