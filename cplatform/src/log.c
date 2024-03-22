#include "log.h"

#include "c_time.h"
#include "platform.h"

#include <string.h>

#define DEFAULT_LEVEL  LOG_WARN
#define DEFAULT_QUIET  0
#define DEFAULT_HEADER 1

static log_t *s_log;

static const char *level_strs[] = { "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };

static const char *level_colors[] = { "\x1b[94m", "\x1b[36m", "\x1b[32m", "\x1b[33m", "\x1b[31m", "\x1b[35m" };

int log_std_cb(log_event_t *ev)
{
	const char *tag_s = "";
	const char *tag_e = "";
	const char *tag	  = "";
	if (ev->tag != NULL) {
		tag_s = "[";
		tag_e = "] ";
		tag   = ev->tag;
	}

	int off = ev->print.off;

	if (ev->header) {
		if (ev->colors) {
			ev->print.off += dprintf(ev->print, "%s %s%-5s\x1b[0m [%s:%s] \x1b[90m%s:%d:\x1b[0m %s%s%s", ev->time, level_colors[ev->level],
						 level_strs[ev->level], ev->pkg, ev->file, ev->func, ev->line, tag_s, tag, tag_e);
		} else {
			ev->print.off += dprintf(ev->print, "%s %-5s [%s:%s] %s:%d: %s%s%s", ev->time, level_strs[ev->level], ev->pkg, ev->file, ev->func, ev->line,
						 tag_s, tag, tag_e);
		}
	} else {
		ev->print.off += dprintf(ev->print, "%s%s%s", tag_s, tag, tag_e);
	}

	ev->print.off += dprintv(ev->print, ev->fmt, ev->ap);
	ev->print.off += dprintf(ev->print, "\n");
	return ev->print.off - off;
}

log_t *log_init(log_t *log)
{
	s_log = log;

	if (s_log == NULL) {
		return NULL;
	}

	s_log->level  = DEFAULT_LEVEL;
	s_log->quiet  = DEFAULT_QUIET;
	s_log->header = DEFAULT_HEADER;

	return log;
}

log_t *log_set(log_t *log)
{
	log_t *cur = s_log;

	s_log = log;
	return cur;
}

const log_t *log_get()
{
	return s_log;
}

const char *log_level_str(int level)
{
	return level_strs[level];
}

int log_set_level(int level)
{
	if (s_log == NULL) {
		return DEFAULT_LEVEL;
	}

	const int cur = s_log->level;

	s_log->level = level;
	return cur;
}

int log_set_quiet(int enable)
{
	if (s_log == NULL) {
		return DEFAULT_QUIET;
	}

	const int quiet = s_log->quiet;

	s_log->quiet = enable;
	return quiet;
}

int log_set_header(int enable)
{
	if (s_log == NULL) {
		return DEFAULT_HEADER;
	}

	const int header = s_log->header;

	s_log->header = enable;
	return header;
}

int log_add_callback(log_cb log, print_dst_t print, int level, int header)
{
	if (s_log == NULL) {
		return 1;
	}

	for (int i = 0; i < LOG_MAX_CALLBACKS; i++) {
		if (!s_log->callbacks[i].log) {
			s_log->callbacks[i] = (log_callback_t){
				.log	= log,
				.print	= print,
				.level	= level,
				.header = header,
			};
			return 0;
		}
	}
	return 1;
}

static int init_event(log_event_t *ev, print_dst_t print, int colors, int header)
{
	if (!ev->time[0]) {
		c_time_str(ev->time);
	}
	ev->print  = print;
	ev->colors = colors;
	ev->header = header;
	return 0;
}

int log_log(int level, const char *pkg, const char *file, const char *func, int line, const char *tag, const char *fmt, ...)
{
	if (s_log == NULL || file == NULL || fmt == NULL) {
		return 1;
	}

	log_event_t ev = {
		.pkg   = pkg,
		.file  = file,
		.func  = func,
		.tag   = tag,
		.fmt   = fmt,
		.line  = line,
		.level = level,
	};

	if (!s_log->quiet && level >= s_log->level) {
		init_event(&ev, PRINT_DST_FILE(stderr), 1, s_log->header);
		va_start(ev.ap, fmt);
		log_std_cb(&ev);
		va_end(ev.ap);
	}

	for (int i = 0; i < LOG_MAX_CALLBACKS && s_log->callbacks[i].log; i++) {
		log_callback_t *cb = &s_log->callbacks[i];
		if (level >= cb->level) {
			init_event(&ev, cb->print, 0, cb->header);
			va_start(ev.ap, fmt);
			cb->print.off += cb->log(&ev);
			va_end(ev.ap);
		}
	}
	return 0;
}

const char *log_strerror(int errnum)
{
#if defined C_WIN
	static char buf[256] = { 0 };
	strerror_s(buf, sizeof(buf), errnum);
	return buf;
#else
	return strerror(errnum);
#endif
}
