#include "test_cplatform.h"

#include "c_time.h"
#include "cplatform.h"
#include "log.h"
#include "platform.h"

#include <errno.h>
#include <string.h>

#define EXPECT(_actual, _expected)                                                                   \
	if ((_actual) != (_expected)) {                                                              \
		log_error("cplatform", "test", NULL, "%s = %d != %d", #_actual, _actual, _expected); \
		ret = 1;                                                                             \
	}

#define EXPECT_STR(_actual, _expected)                                                               \
	if (strcmp(_actual, _expected) != 0) {                                                       \
		log_error("cplatform", "test", NULL, "%s = %s != %s", #_actual, _actual, _expected); \
		ret = 1;                                                                             \
	}

#define EXPECT_WSTR(_actual, _expected)                                                                              \
	if (wcscmp(_actual, _expected) != 0) {                                                                       \
		log_error("cplatform", "test", NULL, "%s = %s != %s", #_actual, (char *)_actual, (char *)_expected); \
		ret = 1;                                                                                             \
	}

static FILE *file_open(const char *path, const char *mode)
{
	FILE *file = NULL;
#if defined(C_WIN)
	fopen_s(&file, path, mode);
#else
	file = fopen(path, mode);
#endif
	return file;
}

static size_t file_read(FILE *file, size_t size, void *data, size_t data_size)
{
	size_t cnt;

#if defined(C_WIN)
	cnt = fread_s(data, data_size, size, 1, file);
#else
	(void)data_size;
	cnt = fread(data, size, 1, file);
#endif
	if (cnt != 1) {
		return 0;
	}

	return size;
}

static int file_delete(const char *path)
{
	int ret;
#if defined(C_WIN)
	ret = DeleteFileA(path) == 0 ? 1 : 0;
#else
	errno = 0;
	ret   = remove(path);
#endif
	return ret;
}

static void line(int rh, int rc)
{
	for (int i = 0; i < rh + 1; i++) {
		c_printf("-");
	}

	for (int j = 0; j < 2; j++) {
		c_printf("+");

		for (int i = 0; i < rc + 2; i++) {
			c_printf("-");
		}
	}

	c_printf("\n");
}

static int t_init_free()
{
	int ret = 0;

	EXPECT(cplatform_init(NULL), NULL);
	EXPECT(cplatform_free(NULL), 1);

	return ret;
}

static int t_time()
{
	int ret = 0;

	c_time();
	c_sleep(1);
	c_time_str(NULL);

	return ret;
}

static int print_callback(log_event_t *ev)
{
	(void)ev;
	return 0;
}

static int t_log()
{
	int ret = 0;

	const log_t *log = log_get();

	log_t tmp = *log;
	log_set(&tmp);

	EXPECT(log_add_callback(log_std_cb, PRINT_DST_NONE(), LOG_TRACE, 1), 0);

	for (int i = 1; i < LOG_MAX_CALLBACKS; i++) {
		EXPECT(log_add_callback(print_callback, PRINT_DST_NONE(), LOG_TRACE, 1), 0);
	}

	EXPECT(log_add_callback(print_callback, PRINT_DST_NONE(), LOG_TRACE, 1), 1);

	int quiet  = log_set_quiet(0);
	int level  = log_set_level(LOG_TRACE);
	int header = log_set_header(0);
	log_trace("test_cplatform", "main", NULL, "");
	log_set_header(header);

	log_trace(NULL, NULL, NULL, NULL);
	log_trace("test_cplatform", "main", "test", "trace");
	log_debug("test_cplatform", "main", "test", "debug");
	log_info("test_cplatform", "main", "test", "info");
	log_warn("test_cplatform", "main", "test", "warn");
	log_error("test_cplatform", "main", "test", "error");
	log_fatal("test_cplatform", "main", "test", "fatal");

	log_set_level(level);
	log_set_quiet(quiet);

	log_set(NULL);
	EXPECT(log_add_callback(print_callback, PRINT_DST_NONE(), LOG_TRACE, 1), 1);
	log_set_level(LOG_FATAL);
	log_set_quiet(0);
	log_set_header(0);
	log_init(NULL);
	log_level_str(LOG_TRACE);

	log_set((log_t *)log);

	return ret;
}

static int t_print()
{
	int ret = 0;

	EXPECT(c_printv(NULL, NULL), 0);
	EXPECT(c_fprintv(NULL, NULL, NULL), 0);
	const char *temp = "temp.txt";

	FILE *file = file_open(temp, "wb+");
	fclose(file);
	file = file_open(temp, "r");

	int level = log_set_level(LOG_FATAL);
	EXPECT(c_fprintf(file, "Test"), 0);
	log_set_level(level);

	fclose(file);
	file_delete(temp);

	EXPECT(c_sprintv(NULL, 0, 0, NULL, NULL), 0);
#ifdef C_LINUX
	char cbuf[2] = { 0 };
	EXPECT(c_sprintf(cbuf, sizeof(cbuf), 0, "abc"), 0);
#endif
	EXPECT(c_wprintv(NULL, NULL), 0);
	EXPECT(c_fwprintv(NULL, NULL, NULL), 0);
	EXPECT(c_swprintv(NULL, 0, 0, NULL, NULL), 0);
#ifdef C_LINUX
	wchar wbuf[2] = { 0 };
	EXPECT(c_swprintf(wbuf, sizeof(wbuf), 0, L"abc"), 0);
#endif
	EXPECT(c_setmode(NULL, 0), 0);
	EXPECT(c_fflush(NULL), 1);
	EXPECT(c_v(PRINT_DST_NONE()), 0);
	EXPECT(c_vr(PRINT_DST_NONE()), 0);
	EXPECT(c_ur(PRINT_DST_NONE()), 0);
	EXPECT(c_wv(PRINT_DST_WNONE()), 0);
	EXPECT(c_wvr(PRINT_DST_WNONE()), 0);
	EXPECT(c_wur(PRINT_DST_WNONE()), 0);

	return ret;
}

static int t_char()
{
	int ret = 0;

	c_fflush(stdout);
	c_fflush(stderr);
	c_printf("\n");
	c_fflush(stdout);
	c_fflush(stderr);

	const int rh  = 6;
	const int rc  = 6;
	const int hrc = (rc - 6) / 2;

	c_printf("%*s | %-*s | %-*s\n", rh, "", rc, "char", rc, "wchar");

	line(rh, rc);

	c_printf("%*s | %*s", rh, "print", hrc, "");
	EXPECT(c_v(PRINT_DST_STD()), 4);
	EXPECT(c_vr(PRINT_DST_STD()), 6);
	EXPECT(c_ur(PRINT_DST_STD()), 6);
	c_printf("%*s | %*s", hrc, "", hrc, "");
	EXPECT(c_wv(PRINT_DST_WSTD()), 2);
	EXPECT(c_wvr(PRINT_DST_WSTD()), 2);
	EXPECT(c_wur(PRINT_DST_WSTD()), 2);
	c_wprintf(L"\n");
	c_fflush(stdout);

	line(rh, rc);

	c_printf("%*s | %*s", rh, "stdout", hrc, "");
	EXPECT(c_v(PRINT_DST_FILE(stdout)), 4);
	EXPECT(c_vr(PRINT_DST_FILE(stdout)), 6);
	EXPECT(c_ur(PRINT_DST_FILE(stdout)), 6);
	c_printf("%*s | %*s", hrc, "", hrc, "");
	EXPECT(c_wv(PRINT_DST_WFILE(stdout)), 2);
	EXPECT(c_wvr(PRINT_DST_WFILE(stdout)), 2);
	EXPECT(c_wur(PRINT_DST_WFILE(stdout)), 2);
	c_printf("\n");

	line(rh, rc);
	c_fflush(stdout);

	c_fprintf(stderr, "%*s | %*s", rh, "stderr", hrc, "");
	EXPECT(c_v(PRINT_DST_FILE(stderr)), 4);
	EXPECT(c_vr(PRINT_DST_FILE(stderr)), 6);
	EXPECT(c_ur(PRINT_DST_FILE(stderr)), 6);
	c_fprintf(stderr, "%*s | %*s", hrc, "", hrc, "");
	EXPECT(c_wv(PRINT_DST_WFILE(stderr)), 2);
	EXPECT(c_wvr(PRINT_DST_WFILE(stderr)), 2);
	EXPECT(c_wur(PRINT_DST_WFILE(stderr)), 2);
	c_fflush(stderr);
	c_fprintf(stdout, "\n\n");
	c_fflush(stdout);

	return ret;
}

static int t_file()
{
	int ret = 0;

	const char exp[] = "│ \r\n├─\r\n└─\r\n";

	const char *path = "char.txt";

	FILE *file = file_open(path, "wb+");

	EXPECT(c_v(PRINT_DST_FILE(file)), 4);
	c_fprintf(file, "\r\n");
	EXPECT(c_vr(PRINT_DST_FILE(file)), 6);
	c_fprintf(file, "\r\n");
	EXPECT(c_ur(PRINT_DST_FILE(file)), 6);
	c_fprintf(file, "\r\n");

	fclose(file);
	file	      = file_open(path, "rb+");
	char data[64] = { 0 };
	EXPECT(file_read(file, sizeof(exp) - 1, data, sizeof(data)), sizeof(exp) - 1);
	fclose(file);
	file_delete(path);

	EXPECT_STR(data, exp);

	char buf[64] = { 0 };
	int off	     = 0;
	EXPECT(c_v(PRINT_DST_BUF(buf, sizeof(buf), off)), 4);
	off += 4;
	off += c_sprintf(buf, sizeof(buf), off, "\r\n");
	EXPECT(c_vr(PRINT_DST_BUF(buf, sizeof(buf), off)), 6);
	off += 6;
	off += c_sprintf(buf, sizeof(buf), off, "\r\n");
	EXPECT(c_ur(PRINT_DST_BUF(buf, sizeof(buf), off)), 6);
	off += 6;
	off += c_sprintf(buf, sizeof(buf), off, "\r\n");

	EXPECT_STR(buf, exp);
	return ret;
}

static int t_wfile()
{
	int ret = 0;

	const wchar exp[] = L"\u2502 \r\n\u251C\u2500\r\n\u2514\u2500\r\n";

	const char *path = "wchar.txt";

	FILE *file = file_open(path, "wb+");

	EXPECT(c_wv(PRINT_DST_WFILE(file)), 2);
	c_fwprintf(file, L"\n");
	EXPECT(c_wvr(PRINT_DST_WFILE(file)), 2);
	c_fwprintf(file, L"\n");
	EXPECT(c_wur(PRINT_DST_WFILE(file)), 2);
	c_fwprintf(file, L"\n");

	fclose(file);

	file	       = file_open(path, "rb+");
	wchar data[64] = { 0 };
	file_read(file, sizeof(exp) - 1, data, sizeof(data));
	//EXPECT_WSTR(data, exp); //TODO: read wchar from file
	fclose(file);
	file_delete(path);

	wchar buf[64] = { 0 };
	int off	      = 0;
	EXPECT(c_wv(PRINT_DST_WBUF(buf, sizeof(buf), off)), 2);
	off += 2;
	off += c_swprintf(buf, sizeof(buf), off, L"\r\n");
	EXPECT(c_wvr(PRINT_DST_WBUF(buf, sizeof(buf), off)), 2);
	off += 2;
	off += c_swprintf(buf, sizeof(buf), off, L"\r\n");
	EXPECT(c_wur(PRINT_DST_WBUF(buf, sizeof(buf), off)), 2);
	off += 2;
	off += c_swprintf(buf, sizeof(buf), off, L"\r\n");

	EXPECT_WSTR(buf, exp);

	return ret;
}

int test_cplatform()
{
	int ret = 0;

	EXPECT(t_init_free(), 0);
	EXPECT(t_time(), 0);
	EXPECT(t_log(), 0);
	EXPECT(t_print(), 0);
	EXPECT(t_char(), 0);
	EXPECT(t_file(), 0);
	EXPECT(t_wfile(), 0);

	return ret;
}
