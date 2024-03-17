#ifndef CPLATFORM_H
#define CPLATFORM_H

#include "log.h"

typedef struct cplatform_s {
	log_t log;
} cplatform_t;

cplatform_t *cplatform_init(cplatform_t *cplatform);
int cplatform_free(cplatform_t *cplatform);

#endif
