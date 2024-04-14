#ifndef PRINT_H
#define PRINT_H

#include "pdef.h"
#include "type.h"

#include <stdarg.h>
#include <stdio.h>

PLTAPI int c_print_init();

PLTAPI int c_printv(const char *fmt, va_list args);
PLTAPI int c_printf(const char *fmt, ...);

PLTAPI int c_fprintv(FILE *file, const char *fmt, va_list args);
PLTAPI int c_fprintf(FILE *file, const char *fmt, ...);

PLTAPI int c_sprintv(char *buf, size_t size, int off, const char *fmt, va_list args);
PLTAPI int c_sprintf(char *buf, size_t size, int off, const char *fmt, ...);

PLTAPI int c_wprintv(const wchar *fmt, va_list args);
PLTAPI int c_wprintf(const wchar *fmt, ...);

PLTAPI int c_fwprintv(FILE *file, const wchar *fmt, va_list args);
PLTAPI int c_fwprintf(FILE *file, const wchar *fmt, ...);

PLTAPI int c_swprintv(wchar *buf, size_t size, int off, const wchar *fmt, va_list args);
PLTAPI int c_swprintf(wchar *buf, size_t size, int off, const wchar *fmt, ...);

PLTAPI int c_fflush(FILE *file);

PLTAPI int c_setmode(FILE *file, int mode);
PLTAPI int c_setmodew(FILE *file);

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

PLTAPI int c_printv_cb(print_dst_t dst, const char *fmt, va_list args);
PLTAPI int c_sprintv_cb(print_dst_t dst, const char *fmt, va_list args);
PLTAPI int c_fprintv_cb(print_dst_t dst, const char *fmt, va_list args);

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

PLTAPI int c_wprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args);
PLTAPI int c_swprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args);
PLTAPI int c_fwprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args);

PLTAPI int dprintv(print_dst_t dst, const char *fmt, va_list args);
PLTAPI int dprintf(print_dst_t dst, const char *fmt, ...);

PLTAPI int dwprintv(wprint_dst_t dst, const wchar *fmt, va_list args);
PLTAPI int dwprintf(wprint_dst_t dst, const wchar *fmt, ...);

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
