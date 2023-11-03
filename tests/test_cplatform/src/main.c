#include "log.h"
#include "platform.h"

#include <string.h>

#define V  L"\u2502 "
#define VR L"\u251C\u2500"
#define UR L"\u2514\u2500"

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

FILE *file_open(const char *path, const char *mode)
{
	FILE *file = NULL;
#if defined(C_WIN)
	fopen_s(&file, path, mode);
#else
	file = fopen(path, mode);
#endif
	return file;
}

size_t file_read(FILE *file, size_t size, void *data, size_t data_size)
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

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	int ret = 0;

	log_t log = { 0 };
	log_init(&log);

	log_set_level(LOG_TRACE);

	SetConsoleOutputCP(CP_UTF8);
	const wchar_t unicodeString[] = L"├─\n";
	WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), unicodeString, wcslen(unicodeString), NULL, NULL);

	c_print_init();

	log_trace("test_cplatform", "main", NULL, "trace");
	log_debug("test_cplatform", "main", NULL, "debug");
	log_info("test_cplatform", "main", NULL, "info");
	log_warn("test_cplatform", "main", NULL, "warn");
	log_error("test_cplatform", "main", NULL, "error");
	log_fatal("test_cplatform", "main", NULL, "fatal");

	c_printf("\n");

	c_fflush(stdout);
	c_fflush(stderr);

	log_info("test_cplatform", "main", NULL, "print start");

	c_fflush(stderr);

	const int rh  = 6;
	const int rc  = 6;
	const int hrc = (rc - 6) / 2;

	c_printf("\n");

	c_printf("%*s | %-*s | %-*s\n", rh, "", rc, "char", rc, "wchar");

	line(rh, rc);

	c_printf("%*s | %*s", rh, "print", hrc, "");
	EXPECT(c_v(c_printf_cb, 0, 0, NULL), 2);
	EXPECT(c_vr(c_printf_cb, 0, 0, NULL), 2);
	EXPECT(c_ur(c_printf_cb, 0, 0, NULL), 2);
	c_printf("%*s | %*s", hrc, "", hrc, "");
	EXPECT(c_wprintf_cb(NULL, 0, 0, V), 2);
	EXPECT(c_wprintf_cb(NULL, 0, 0, VR), 2);
	EXPECT(c_wprintf_cb(NULL, 0, 0, UR), 2);
	c_printf("\n");
	c_fflush(stdout);

	line(rh, rc);

	c_printf("%*s | %*s", rh, "stdout", hrc, "");
	EXPECT(c_v(c_fprintf_cb, 0, 0, stdout), 2);
	EXPECT(c_vr(c_fprintf_cb, 0, 0, stdout), 2);
	EXPECT(c_ur(c_fprintf_cb, 0, 0, stdout), 2);
	c_printf("%*s | %*s", hrc, "", hrc, "");
	EXPECT(c_fwprintf_cb(stdout, 0, 0, V), 2);
	EXPECT(c_fwprintf_cb(stdout, 0, 0, VR), 2);
	EXPECT(c_fwprintf_cb(stdout, 0, 0, UR), 2);
	c_printf("\n");
	c_fflush(stdout);

	line(rh, rc);

	c_printf("%*s | %*s", rh, "stderr", hrc, "");
	EXPECT(c_v(c_fprintf_cb, 0, 0, stderr), 2);
	EXPECT(c_vr(c_fprintf_cb, 0, 0, stderr), 2);
	EXPECT(c_ur(c_fprintf_cb, 0, 0, stderr), 2);
	c_printf("%*s | %*s", hrc, "", hrc, "");
	EXPECT(c_fwprintf_cb(stderr, 0, 0, V), 2);
	EXPECT(c_fwprintf_cb(stderr, 0, 0, VR), 2);
	EXPECT(c_fwprintf_cb(stderr, 0, 0, UR), 2);
	c_fflush(stderr);
	c_printf("\n");

	{
		const char exp[] = "\xB3 \r\n\xC3\xC4\r\n\xC0\xC4\r\n";

		const char *path = "char.txt";

		FILE *file = file_open(path, "wb+");

		fpos_t pos;
		fgetpos(file, &pos);

		EXPECT(c_v(c_fprintf_cb, 0, 0, file), 2);
		c_fprintf(file, "\r\n");
		EXPECT(c_vr(c_fprintf_cb, 0, 0, file), 2);
		c_fprintf(file, "\r\n");
		EXPECT(c_ur(c_fprintf_cb, 0, 0, file), 2);
		c_fprintf(file, "\r\n");

		c_fflush(file);
		fsetpos(file, &pos);

		char data[64] = { 0 };
		file_read(file, sizeof(exp), data, sizeof(data));

		fclose(file);

		EXPECT_STR(data, exp);

		char buf[32] = { 0 };

		int off = 0;

		EXPECT(c_v(c_sprintf_cb, sizeof(buf), off, buf), 2);
		off += 2;
		off += c_sprintf(buf, sizeof(buf), off, "\r\n");
		EXPECT(c_vr(c_sprintf_cb, sizeof(buf), off, buf), 2);
		off += 2;
		off += c_sprintf(buf, sizeof(buf), off, "\r\n");
		EXPECT(c_ur(c_sprintf_cb, sizeof(buf), off, buf), 2);
		off += 2;
		off += c_sprintf(buf, sizeof(buf), off, "\r\n");

		EXPECT_STR(buf, exp);
	}

	{
		const wchar exp[] = V L"\r\n" VR L"\r\n" UR L"\r\n";

		const char *path = "wchar.txt";

		FILE *file = file_open(path, "wb+");

		fpos_t pos;
		fgetpos(file, &pos);

		EXPECT(c_fwprintf_cb(file, 0, 0, V), 2);
		c_fwprintf(file, L"\n");
		EXPECT(c_fwprintf_cb(file, 0, 0, VR), 2);
		c_fwprintf(file, L"\n");
		EXPECT(c_fwprintf_cb(file, 0, 0, UR), 2);
		c_fwprintf(file, L"\n");

		c_fflush(file);
		fsetpos(file, &pos);

		wchar data[64] = { 0 };
		file_read(file, sizeof(exp), data, sizeof(data));

		EXPECT_WSTR(data, exp);

		fclose(file);

		wchar buf[64] = { 0 };

		int off = 0;
		EXPECT(c_swprintf_cb(buf, sizeof(buf), off, V), 2);
		off += 2;
		off += c_swprintf(buf, sizeof(buf), off, L"\r\n");
		EXPECT(c_swprintf_cb(buf, sizeof(buf), off, VR), 2);
		off += 2;
		off += c_swprintf(buf, sizeof(buf), off, L"\r\n");
		EXPECT(c_swprintf_cb(buf, sizeof(buf), off, UR), 2);
		off += 2;
		off += c_swprintf(buf, sizeof(buf), off, L"\r\n");

		EXPECT_WSTR(buf, exp);
	}

	c_printf("\n");

	log_info("test_cplatform", "main", NULL, "print end");

	return ret;
}
