#ifndef C_TIME_H
#define C_TIME_H

#include "type.h"

#define C_TIME_BUF_SIZE 24

u64 c_time();
const char *c_time_str(char *buf);

int c_sleep(u32 milliseconds);

#endif
