#include "mem.h"

#include "log.h"
#include "platform.h"

#include <memory.h>
#include <stdlib.h>

static mem_t *s_mem;
static int s_oom;

mem_t *mem_init(mem_t *mem)
{
	s_mem = mem;
	s_oom = 0;

	return mem;
}

mem_t *mem_sset(mem_t *mem)
{
	s_mem = mem;
	return mem;
}

const mem_t *mem_get()
{
	return s_mem;
}

static void get_max_unit(size_t *size, char *u)
{
	if (*size > 1024) {
		*size /= 1024;
		*u = 'K';
	}

	if (*size > 1024) {
		*size /= 1024;
		*u = 'M';
	}
}

static int print_mem(size_t mem, print_dst_t dst)
{
	size_t umem = mem;

	char u = '\0';

	get_max_unit(&umem, &u);

	int off = dst.off;

	if (u == '\0') {
		dst.off += dprintf(dst, "%zu B\n", mem);
	} else {
		dst.off += dprintf(dst, "%zu %cB (%zu B)\n", umem, u, mem);
	}

	return dst.off - off;
}

int mem_print(print_dst_t dst)
{
	if (s_mem == NULL) {
		return 0;
	}

	int off = dst.off;

	dst.off += dprintf(dst, "memory stats:\n");
	dst.off += dprintf(dst, "    unfreed:  ");
	dst.off += print_mem(s_mem->mem, dst);
	dst.off += dprintf(dst, "    peak:     ");
	dst.off += print_mem(s_mem->peak, dst);
	dst.off += dprintf(dst, "    total:    ");
	dst.off += print_mem(s_mem->total, dst);
	dst.off += dprintf(dst, "    allocs:   %d\n", s_mem->allocs);
	dst.off += dprintf(dst, "    reallocs: %d\n", s_mem->reallocs);

	return dst.off - off;
}

int mem_check()
{
	if (s_mem->mem == 0) {
		return 0;
	}

	log_warn("cutils", "mem", NULL, "%d bytes were not freed", s_mem->mem);
	return 1;
}

#define MAX(a, b) (a) > (b) ? (a) : (b)

void *mem_alloc(size_t size)
{
	if (size == 0) {
		log_warn("cutils", "mem", NULL, "malloc 0 bytes");
	}

	void *ptr = size > 0 && s_oom ? NULL : malloc(size);

	if (ptr == NULL) {
		log_error("cutils", "mem", NULL, "out of memory");
		return NULL;
	}

	if (s_mem) {
		s_mem->mem += size;
		s_mem->peak = MAX(s_mem->mem, s_mem->peak);
		s_mem->total += size;
		s_mem->allocs++;
	}

	return ptr;
}

void *mem_calloc(size_t count, size_t size)
{
	if (size == 0) {
		log_warn("cutils", "mem", NULL, "calloc 0 bytes");
	}

	void *ptr = count * size > 0 && s_oom ? NULL : calloc(count, size);

	if (ptr == NULL) {
		log_error("cutils", "mem", NULL, "out of memory");
		return NULL;
	}

	if (s_mem) {
		s_mem->mem += count * size;
		s_mem->peak = MAX(s_mem->mem, s_mem->peak);
		s_mem->total += size;
		s_mem->allocs++;
	}

	return ptr;
}

void *mem_realloc(void *memory, size_t new_size, size_t old_size)
{
	if (memory == NULL) {
		log_error("cutils", "mem", NULL, "realloc NULL");
		return NULL;
	}

	if (new_size == 0 || new_size == old_size) {
		log_warn("cutils", "mem", NULL, "realloc %zu -> %zu bytes", old_size, new_size);
	} else {
		log_trace("cutils", "mem", NULL, "realloc %zu -> %zu bytes", old_size, new_size);
	}

	if (new_size == 0) {
		return memory;
	}

	if (old_size == 0) {
		return mem_alloc(new_size);
	}

	void *ptr = new_size > old_size && s_oom ? NULL : realloc(memory, new_size);

	if (ptr == NULL) {
		log_error("cutils", "mem", NULL, "out of memory");
		return NULL;
	}

	if (s_mem) {
		s_mem->mem -= old_size;
		s_mem->mem += new_size;
		s_mem->peak = MAX(s_mem->mem, s_mem->peak);
		s_mem->total += new_size - old_size;
		s_mem->reallocs++;
	}

	return ptr;
}

void *mem_set(void *dst, int val, size_t size)
{
	if (dst == NULL) {
		return NULL;
	}

	return memset(dst, val, size);
}

void *mem_cpy(void *dst, size_t size, const void *src, size_t len)
{
	if (len > size) {
		log_error("cutils", "mem", NULL, "destination too small %d/%d", size, len);
		return NULL;
	}
#if defined(C_WIN)
	memcpy_s(dst, size, src, len);
#else
	memcpy(dst, src, len);
#endif
	return dst;
}

int mem_cmp(const void *l, const void *r, size_t size)
{
	return memcmp(l, r, size);
}

int mem_swap(void *ptr1, void *ptr2, size_t size)
{
	if (ptr1 == NULL || ptr2 == NULL) {
		return 1;
	}

	unsigned char *byte_ptr1 = (unsigned char *)ptr1;
	unsigned char *byte_ptr2 = (unsigned char *)ptr2;

	for (size_t i = 0; i < size; i++) {
		unsigned char temp = byte_ptr1[i];
		byte_ptr1[i]	   = byte_ptr2[i];
		byte_ptr2[i]	   = temp;
	}

	return 0;
}

void mem_free(void *memory, size_t size)
{
	if (memory == NULL) {
		return;
	}

	if (s_mem) {
		s_mem->mem -= size;
	}

	free(memory);
}

void mem_oom(int oom)
{
	s_oom = oom;
}
