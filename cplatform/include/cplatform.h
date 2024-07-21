#ifndef CPLATFORM_H
#define CPLATFORM_H

#include "log.h"
#include "mem.h"

typedef struct cplatform_s {
	log_t log;
	mem_t mem;
} cplatform_t;

PLTAPI cplatform_t *cplatform_init(cplatform_t *cplatform);
PLTAPI int cplatform_free(cplatform_t *cplatform);

#endif
