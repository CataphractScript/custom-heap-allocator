#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

int hinit(size_t size);
void *halloc(size_t size);
void hfree(void *ptr);

void heap_gc();

#endif
