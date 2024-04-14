#ifndef C_TIME_H
#define C_TIME_H

#include "pdef.h"
#include "type.h"

#define C_TIME_BUF_SIZE 24

PLTAPI u64 c_time();
PLTAPI const char *c_time_str(char *buf);

PLTAPI int c_sleep(u32 milliseconds);

#endif
