#include "cplatform.h"

cplatform_t *cplatform_init(cplatform_t *cplatform)
{
	if (cplatform == NULL) {
		return NULL;
	}

	log_init(&cplatform->log);
	mem_init(&cplatform->mem);

	c_print_init();

	return cplatform;
}

int cplatform_free(cplatform_t *cplatform)
{
	if (cplatform == NULL) {
		return 1;
	}

	return 0;
}
