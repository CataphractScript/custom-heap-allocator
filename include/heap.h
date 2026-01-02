#ifndef HEAP_H
#define HEAP_H

#include <stddef.h>
#include <stdint.h>

int hinit(size_t size);
void *halloc(size_t size);
void hfree(void *ptr);

void heap_gc();
int detect_heap_spray(void *ptr);
void memory_pool_init(size_t pool_size);

#endif
