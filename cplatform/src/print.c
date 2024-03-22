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
	if (ret < 0) {
		int errnum = errno;
		if (errnum == 0 && (file == stdout || file == stderr)) {
			file_reopen(NULL, "w", file);
			ret = vfprintf(file, fmt, copy);
		} else {
			log_error("cplatform", "print", NULL, "failed to write to file: %s (%d)", log_strerror(errnum), errnum);
			ret = 0;
		}
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
	if (buf == NULL && size == 0) {
		ret = vsnprintf(buf, size / sizeof(char) - off, fmt, copy);
	} else {
		ret = vsnprintf_s(buf, size / sizeof(char) - off, size / sizeof(char) - off, fmt, copy);
	}
	va_end(copy);
#else
	ret = vsnprintf(buf, size / sizeof(char) - off, fmt, copy);
	va_end(copy);
	if (size > 0 && (size_t)ret > size - off) {
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
	if (ret < 0) {
		int errnum = errno;
		if (errnum == 0 && (file == stdout || file == stderr)) {
			file_reopen(NULL, "w", file);
			ret = vfwprintf(file, fmt, copy);
		} else {
			log_error("cplatform", "print", NULL, "failed to write to file: %s (%d)", log_strerror(errnum), errnum);
			ret = 0;
		}
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
	#pragma warning(push)
	#pragma warning(disable : 6387)
	ret = vswprintf_s(buf, size / sizeof(wchar) - off, fmt, copy);
	#pragma warning(push)
	va_end(copy);
#else
	ret = vswprintf(buf, size / sizeof(wchar) - off, fmt, copy);
	va_end(copy);

	if (size > 0 && (size_t)ret > size - off) {
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

int c_printv_cb(print_dst_t dst, const char *fmt, va_list args)
{
	(void)dst;
	return c_printv(fmt, args);
}

int c_sprintv_cb(print_dst_t dst, const char *fmt, va_list args)
{
	return c_sprintv(dst.out.buf, dst.size, dst.off, fmt, args);
}

int c_fprintv_cb(print_dst_t dst, const char *fmt, va_list args)
{
	int ret = c_fprintv(dst.out.file, fmt, args);
	c_fflush(dst.out.file);
	return ret;
}

int c_wprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args)
{
	(void)dst;
	return c_wprintv(fmt, args);
}

int c_swprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args)
{
	return c_swprintv(dst.out.buf, dst.size, dst.off, fmt, args);
}

int c_fwprintv_cb(wprint_dst_t dst, const wchar *fmt, va_list args)
{
	int ret = c_fwprintv(dst.out.file, fmt, args);
	c_fflush(dst.out.file);
	return ret;
}

int dprintv(print_dst_t dst, const char *fmt, va_list args)
{
	if (dst.cb == NULL) {
		return 0;
	}

	return dst.cb(dst, fmt, args);
}

int dprintf(print_dst_t dst, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = dprintv(dst, fmt, args);
	va_end(args);
	return ret;
}

int dwprintv(wprint_dst_t dst, const wchar *fmt, va_list args)
{
	if (dst.cb == NULL) {
		return 0;
	}

	return dst.cb(dst, fmt, args);
}

int dwprintf(wprint_dst_t dst, const wchar *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = dwprintv(dst, fmt, args);
	va_end(args);
	return ret;
}
