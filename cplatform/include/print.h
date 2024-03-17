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

typedef int (*c_printv_fn)(void *stream, size_t size, int off, const char *fmt, va_list args);
int c_printv_cb(void *buf, size_t size, int off, const char *fmt, va_list args);
int c_sprintv_cb(void *buf, size_t size, int off, const char *fmt, va_list args);
int c_fprintv_cb(void *file, size_t size, int off, const char *fmt, va_list args);

typedef int (*c_wprintv_fn)(void *stream, size_t size, int off, const wchar *fmt, va_list args);
int c_wprintv_cb(void *buf, size_t size, int off, const wchar *fmt, va_list args);
int c_swprintv_cb(void *buf, size_t size, int off, const wchar *fmt, va_list args);
int c_fwprintv_cb(void *file, size_t size, int off, const wchar *fmt, va_list args);

int c_v(c_printv_fn cb, size_t size, int off, void *stream);
int c_vr(c_printv_fn cb, size_t size, int off, void *stream);
int c_ur(c_printv_fn cb, size_t size, int off, void *stream);

int c_wv(c_wprintv_fn cb, size_t size, int off, void *stream);
int c_wvr(c_wprintv_fn cb, size_t size, int off, void *stream);
int c_wur(c_wprintv_fn cb, size_t size, int off, void *stream);

#endif
