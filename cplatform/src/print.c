#include "print.h"

#include "log.h"
#include "platform.h"

#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <wchar.h>

#if defined(C_WIN)
	#include <io.h>
#else
	#include <locale.h>
#endif

static FILE *file_reopen(const char *path, const char *mode, FILE *file)
{
	errno = 0;
#if defined(C_WIN)
	if (path == NULL) {
		return NULL;
	}
	freopen_s(&file, path, mode, file);
#else
	file = freopen(path, mode, file);
#endif
	return file;
}

int c_print_init()
{
	const char *locale = setlocale(LC_ALL, "en_US.UTF-8");
	log_debug("cplatform", "print", NULL, "locale set to %s", locale);
	return 0;
}

int c_printv(const char *fmt, va_list args)
{
	if (fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	errno	= 0;
	int ret = vprintf(fmt, copy);
	if (ret < 0 && errno == 0) {
		file_reopen(NULL, "w", stdout);
		ret = vprintf(fmt, copy);
	}
	va_end(copy);
	return ret;
}

int c_printf(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_printv(fmt, args);
	va_end(args);
	return ret;
}

int c_fprintv(FILE *file, const char *fmt, va_list args)
{
	if (file == NULL || fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	int ret;
	errno = 0;
#if defined(C_WIN)
	ret = vfprintf_s(file, fmt, copy);
#else
	ret = vfprintf(file, fmt, copy);
#endif
	int errnum = errno;
	if (ret < 0 && errnum != 0) {
		log_error("cplatform", "print", NULL, "failed to write to file: %s (%d)", log_strerror(errnum), errnum);
		ret = 0;
	}
	va_end(copy);
	return ret;
}

int c_fprintf(FILE *file, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_fprintv(file, fmt, args);
	va_end(args);
	return ret;
}

int c_sprintv(char *buf, size_t size, int off, const char *fmt, va_list args)
{
	if ((buf == NULL && size > 0) || off * sizeof(char) > size || fmt == NULL) {
		return 0;
	}

	buf = buf == NULL ? buf : &buf[off];

	va_list copy;
	va_copy(copy, args);
	int ret;
#if defined(C_WIN)
	ret = vsnprintf_s(buf, size / sizeof(char) - off, size / sizeof(char) - off, fmt, copy);
	va_end(copy);
#else
	ret = vsnprintf(buf, size / sizeof(char) - off, fmt, copy);
	va_end(copy);
	if (size > 0 && (size_t)ret > size) {
		return 0;
	}
#endif
	return ret;
}

int c_sprintf(char *buf, size_t size, int off, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_sprintv(buf, size, off, fmt, args);
	va_end(args);
	return ret;
}

int c_wprintv(const wchar *fmt, va_list args)
{
	if (fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);

	int mode = c_setmodew(stdout);

	errno	= 0;
	int ret = vwprintf(fmt, copy);
	if (ret < 0 && errno == 0) {
		file_reopen(NULL, "w", stdout);
		ret = vwprintf(fmt, copy);
	}

	c_setmode(stdout, mode);

	va_end(copy);
	return ret;
}

int c_wprintf(const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_wprintv(fmt, args);
	va_end(args);
	return ret;
}

int c_fwprintv(FILE *file, const wchar *fmt, va_list args)
{
	if (file == NULL || fmt == NULL) {
		return 0;
	}

	va_list copy;
	va_copy(copy, args);
	int ret;
	errno = 0;
#if defined(C_WIN)
	int mode = c_setmodew(file);
	ret	 = vfwprintf_s(file, fmt, copy);
	c_setmode(file, mode);
#else
	ret = vfwprintf(file, fmt, copy);
#endif
	if (ret < 0 && errno == 0) {
		file_reopen(NULL, "w", file);
		ret = vfwprintf(file, fmt, copy);
	}
	va_end(copy);
	return ret;
}

int c_fwprintf(FILE *file, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_fwprintv(file, fmt, args);
	va_end(args);
	return ret;
}

int c_swprintv(wchar *buf, size_t size, int off, const wchar *fmt, va_list args)
{
	if ((buf == NULL && size > 0) || off * sizeof(wchar) > size || fmt == NULL) {
		return 0;
	}

	buf = buf == NULL ? buf : &buf[off];

	va_list copy;
	va_copy(copy, args);
	int ret;
#if defined(C_WIN)
	ret = vswprintf_s(buf, size / sizeof(wchar) - off, fmt, copy);
	va_end(copy);
#else
	ret = vswprintf(buf, size / sizeof(wchar) - off, fmt, copy);
	va_end(copy);

	if (size > 0 && (size_t)ret > size) {
		return 0;
	}
#endif
	return ret;
}

int c_swprintf(wchar *buf, size_t size, int off, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = c_swprintv(buf, size, off, fmt, args);
	va_end(args);
	return ret;
}

int c_fflush(FILE *file)
{
	if (file == NULL) {
		return 1;
	}

	return fflush(file);
}

int c_setmode(FILE *file, int mode)
{
	if (file == NULL) {
		return mode;
	}

	c_fflush(file);

	int ret;

#if defined(C_WIN)
	errno = 0;

	ret = _setmode(_fileno(file), mode);
	if (ret < 0) {
		int errnum = errno;
		log_error("cplatform", "print", NULL, "failed to set mode to: %d: %s (%d)", mode, log_strerror(errnum), errnum);
	}
#else
	ret = mode;
#endif

	return ret;
}

int c_setmodew(FILE *file)
{
#if defined(C_WIN)
	return c_setmode(file, _O_WTEXT);
#else
	fflush(file);
	return 0;
#endif
}

int c_printv_cb(void *buf, size_t size, int off, const char *fmt, va_list args)
{
	(void)buf;
	(void)size;
	(void)off;
	return c_printv(fmt, args);
}

int c_sprintv_cb(void *buf, size_t size, int off, const char *fmt, va_list args)
{
	return c_sprintv(buf, size, off, fmt, args);
}

int c_fprintv_cb(void *priv, size_t size, int off, const char *fmt, va_list args)
{
	(void)size;
	(void)off;
	return c_fprintv(priv, fmt, args);
}

int c_printf_cb(void *buf, size_t size, int off, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_printv_cb(buf, size, off, fmt, args);
	va_end(args);
	return len;
}

int c_sprintf_cb(void *buf, size_t size, int off, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_sprintv_cb(buf, size - off * sizeof(char), off, fmt, args);
	va_end(args);
	return len;
}

int c_fprintf_cb(void *priv, size_t size, int off, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_fprintv_cb(priv, size, off, fmt, args);
	va_end(args);
	return len;
}

int c_wprintv_cb(void *buf, size_t size, int off, const wchar *fmt, va_list args)
{
	(void)buf;
	(void)size;
	(void)off;
	return c_wprintv(fmt, args);
}

int c_swprintv_cb(void *buf, size_t size, int off, const wchar *fmt, va_list args)
{
	return c_swprintv(buf, size, off, fmt, args);
}

int c_fwprintv_cb(void *priv, size_t size, int off, const wchar *fmt, va_list args)
{
	(void)size;
	(void)off;
	return c_fwprintv(priv, fmt, args);
}

int c_wprintf_cb(void *buf, size_t size, int off, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_wprintv_cb(buf, size, off, fmt, args);
	va_end(args);
	return len;
}

int c_swprintf_cb(void *buf, size_t size, int off, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_swprintv_cb(buf, size, off, fmt, args);
	va_end(args);
	return len;
}

int c_fwprintf_cb(void *priv, size_t size, int off, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	const int len = c_fwprintv_cb(priv, size, off, fmt, args);
	va_end(args);
	return len;
}

int c_v(c_printf_fn cb, size_t size, int off, void *stream)
{
	if (cb == NULL) {
		return 0;
	}

	return cb(stream, size, off, "│ ");
}

int c_vr(c_printf_fn cb, size_t size, int off, void *stream)
{
	if (cb == NULL) {
		return 0;
	}

	return cb(stream, size, off, "├─");
}

int c_ur(c_printf_fn cb, size_t size, int off, void *stream)
{
	if (cb == NULL) {
		return 0;
	}

	return cb(stream, size, off, "└─");
}

int c_wv(c_wprintf_fn cb, size_t size, int off, void *stream)
{
	if (cb == NULL) {
		return 0;
	}

	return cb(stream, size, off, L"\u2502 ");
}

int c_wvr(c_wprintf_fn cb, size_t size, int off, void *stream)
{
	if (cb == NULL) {
		return 0;
	}

	return cb(stream, size, off, L"\u251C\u2500");
}

int c_wur(c_wprintf_fn cb, size_t size, int off, void *stream)
{
	if (cb == NULL) {
		return 0;
	}

	return cb(stream, size, off, L"\u2514\u2500");
}
