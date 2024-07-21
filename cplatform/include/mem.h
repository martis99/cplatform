#ifndef MEM_H
#define MEM_H

#include "print.h"
#include "type.h"

typedef struct mem_s {
	size_t mem;
	size_t peak;
	size_t total;
	uint allocs;
	uint reallocs;
} mem_t;

mem_t *mem_init(mem_t *mem);
mem_t *mem_sset(mem_t *mem);
const mem_t *mem_get();
int mem_print(print_dst_t dst);
int mem_check();

void *mem_alloc(size_t size);
void *mem_calloc(size_t count, size_t size);
void *mem_realloc(void *memory, size_t new_size, size_t old_size);

void *mem_set(void *dst, int val, size_t size);
void *mem_cpy(void *dst, size_t size, const void *src, size_t len);
int mem_cmp(const void *l, const void *r, size_t size);

int mem_swap(void *ptr1, void *ptr2, size_t size);

void mem_free(void *memory, size_t size);

void mem_oom(int oom);

#endif
