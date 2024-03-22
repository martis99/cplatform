#ifndef PRINT_H
#define PRINT_H

#include "type.h"

#include <stdarg.h>
#include <stdio.h>

int c_print_init();

int c_printv(const char *fmt, va_list args);
int c_printf(const char *fmt, ...);

int c_fprintv(FILE *file, const char *fmt, va_list args);
int c_fprintf(FILE *file, const char *fmt, ...);

int c_sprintv(char *buf, size_t size, int off, const char *fmt, va_list args);
int c_sprintf(char *buf, size_t size, int off, const char *fmt, ...);

int c_wprintv(const wchar *fmt, va_list args);
int c_wprintf(const wchar *fmt, ...);

int c_fwprintv(FILE *file, const wchar *fmt, va_list args);
int c_fwprintf(FILE *file, const wchar *fmt, ...);

int c_swprintv(wchar *buf, size_t size, int off, const wchar *fmt, va_list args);
int c_swprintf(wchar *buf, size_t size, int off, const wchar *fmt, ...);

int c_fflush(FILE *file);

int c_setmode(FILE *file, int mode);
int c_setmodew(FILE *file);

typedef struct print_dst_s print_dst_t;
typedef int (*c_printv_fn)(print_dst_t dst, const char *fmt, va_list args);
struct print_dst_s {
	c_printv_fn cb;
	size_t size;
	int off;
	union {
		char *buf;
		FILE *file;
	} out;
	void *priv;
};

int c_printv_cb(print_dst_t dst, const char *fmt, va_list args);
int c_sprintv_cb(print_dst_t dst, const char *fmt, va_list args);
int c_fprintv_cb(print_dst_t dst, const char *fmt, va_list args);

typedef struct wprint_dst_s wprint_dst_t;
typedef int (*c_wprintv_fn)(wprint_dst_t dst, const wchar *fmt, va_list args);
struct wprint_dst_s {
	c_wprintv_fn cb;
	size_t size;
	int off;
	union {
		wchar *buf;
		FILE *file;
	} out;
	void *priv;
};

int c_wprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args);
int c_swprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args);
int c_fwprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args);

int dprintv(print_dst_t dst, const char *fmt, va_list args);
int dprintf(print_dst_t dst, const char *fmt, ...);

int dwprintv(wprint_dst_t dst, const wchar *fmt, va_list args);
int dwprintf(wprint_dst_t dst, const wchar *fmt, ...);

// clang-format off
#define PRINT_DST_NONE() (print_dst_t) { 0 }
#define PRINT_DST_STD() (print_dst_t) { .cb = c_printv_cb }
#define PRINT_DST_BUF(_buf, _size, _off) (print_dst_t) { .cb = c_sprintv_cb, .out.buf=_buf, .size=_size, .off=_off }
#define PRINT_DST_FILE(_file) (print_dst_t) { .cb=c_fprintv_cb, .out.file=_file }

#define PRINT_DST_WNONE() (wprint_dst_t) { 0 }
#define PRINT_DST_WSTD() (wprint_dst_t) { .cb = c_wprintv_cb }
#define PRINT_DST_WBUF(_wbuf, _size, _off) (wprint_dst_t) { .cb=c_swprintv_cb, .out.buf=_wbuf, .size=_size, .off=_off }
#define PRINT_DST_WFILE(_file) (wprint_dst_t) { .cb=c_fwprintv_cb, .out.file=_file }
// clang-format on

#endif
