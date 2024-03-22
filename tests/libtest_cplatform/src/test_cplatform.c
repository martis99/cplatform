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
	EXPECT(c_fwprintf(file, L"Test"), 0);
	EXPECT(c_fprintf(file, "Test"), 0);
	log_set_level(level);

	fclose(file);
	file_delete(temp);

	EXPECT(c_sprintv(NULL, 0, 0, NULL, NULL), 0);
	EXPECT(dprintf(PRINT_DST_NONE(), NULL), 0);
#ifdef C_LINUX
	char cbuf[2] = { 0 };
	EXPECT(dprintf(PRINT_DST_BUF(cbuf, sizeof(cbuf), 0), "abc"), 0);
#endif

	EXPECT(c_wprintv(NULL, NULL), 0);
	EXPECT(c_fwprintv(NULL, NULL, NULL), 0);
	EXPECT(c_swprintv(NULL, 0, 0, NULL, NULL), 0);
	EXPECT(dwprintf(PRINT_DST_WNONE(), NULL), 0);
#ifdef C_LINUX
	wchar wbuf[2] = { 0 };
	EXPECT(dwprintf(PRINT_DST_WBUF(wbuf, sizeof(wbuf), 0), L"abc"), 0);
#endif

	EXPECT(c_setmode(NULL, 0), 0);
	EXPECT(c_fflush(NULL), 1);

	return ret;
}

static int t_char()
{
	int ret = 0;

	EXPECT(dprintf(PRINT_DST_STD(), "┌─┬─┐"), 5 * 3);
	EXPECT(dwprintf(PRINT_DST_WSTD(), L"\u250C\u2500\u252C\u2500\u2510"), 5);
	EXPECT(dprintf(PRINT_DST_FILE(stdout), "┌─┬─┐"), 5 * 3);
	EXPECT(dwprintf(PRINT_DST_WFILE(stdout), L"\u250C\u2500\u252C\u2500\u2510"), 5);
	c_printf("\n");

	EXPECT(dprintf(PRINT_DST_STD(), "├─┼─┤"), 5 * 3);
	EXPECT(dwprintf(PRINT_DST_WSTD(), L"\u251C\u2500\u253C\u2500\u2524"), 5);
	EXPECT(dprintf(PRINT_DST_FILE(stdout), "├─┼─┤"), 5 * 3);
	EXPECT(dwprintf(PRINT_DST_WFILE(stdout), L"\u251C\u2500\u253C\u2500\u2524"), 5);
	c_printf("\n");

	EXPECT(dprintf(PRINT_DST_STD(), "└─┴─┘"), 5 * 3);
	EXPECT(dwprintf(PRINT_DST_WSTD(), L"\u2514\u2500\u2534\u2500\u2518"), 5);
	EXPECT(dprintf(PRINT_DST_FILE(stdout), "└─┴─┘"), 5 * 3);
	EXPECT(dwprintf(PRINT_DST_WFILE(stdout), L"\u2514\u2500\u2534\u2500\u2518"), 5);
	c_wprintf(L"\n");

	return ret;
}

static int t_file()
{
	int ret = 0;

	const char exp[] = "┼\r\n";
	const char *path = "char.txt";

	FILE *file = file_open(path, "wb+");
	c_fprintf(file, "┼\r\n");
	fclose(file);

	file = file_open(path, "rb+");

	char data[64] = { 0 };
	EXPECT(file_read(file, sizeof(exp) - 1, data, sizeof(data)), sizeof(exp) - 1);
	fclose(file);
	file_delete(path);
	EXPECT_STR(data, exp);

	char buf[64] = { 0 };
	EXPECT(c_sprintf(buf, sizeof(buf), 0, "┼\r\n"), 5);
	EXPECT_STR(buf, exp);

	return ret;
}

static int t_wfile()
{
	int ret = 0;

	const wchar exp[] = L"\u253C\r\n";
	const char *path  = "wchar.txt";

	FILE *file = file_open(path, "wb+");
	c_fwprintf(file, L"\u253C\n");
	fclose(file);

	file = file_open(path, "rb+");

	wchar data[64] = { 0 };
	EXPECT(file_read(file, sizeof(exp) - 1, data, sizeof(data)), 0);
	fclose(file);
	file_delete(path);
	//EXPECT_WSTR(data, exp); //TODO: read wchar from file

	wchar buf[64] = { 0 };
	EXPECT(c_swprintf(buf, sizeof(buf), 0, L"\u253C\r\n"), 3);
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
