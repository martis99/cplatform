#ifndef PRINT_H
#define PRINT_H

#include "type.h"

#include <stdarg.h>
#include <stdio.h>

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

typedef int (*c_printv_fn)(void *priv, size_t size, int off, const char *fmt, va_list args);
int c_printv_cb(void *buf, size_t size, int off, const char *fmt, va_list args);
int c_sprintv_cb(void *buf, size_t size, int off, const char *fmt, va_list args);
int c_fprintv_cb(void *file, size_t size, int off, const char *fmt, va_list args);

typedef int (*c_printf_fn)(void *priv, size_t size, int off, const char *fmt, ...);
int c_printf_cb(void *buf, size_t size, int off, const char *fmt, ...);
int c_sprintf_cb(void *buf, size_t size, int off, const char *fmt, ...);
int c_fprintf_cb(void *file, size_t size, int off, const char *fmt, ...);

typedef int (*c_wprintv_fn)(void *priv, size_t size, int off, const wchar *fmt, va_list args);
int c_wprintv_cb(void *buf, size_t size, int off, const wchar *fmt, va_list args);
int c_swprintv_cb(void *buf, size_t size, int off, const wchar *fmt, va_list args);
int c_fwprintv_cb(void *file, size_t size, int off, const wchar *fmt, va_list args);

typedef int (*c_wprintf_fb)(void *priv, size_t size, int off, const wchar *fmt, ...);
int c_wprintf_cb(void *buf, size_t size, int off, const wchar *fmt, ...);
int c_swprintf_cb(void *buf, size_t size, int off, const wchar *fmt, ...);
int c_fwprintf_cb(void *file, size_t size, int off, const wchar *fmt, ...);

int c_v(c_printf_fn cb, size_t size, int off, void *priv);
int c_vr(c_printf_fn cb, size_t size, int off, void *priv);
int c_ur(c_printf_fn cb, size_t size, int off, void *priv);

#endif
