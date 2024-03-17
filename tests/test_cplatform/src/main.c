#include "cplatform.h"
#include "test_cplatform.h"

int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	cplatform_t cplatform = { 0 };
	cplatform_init(&cplatform);

	int ret = test_cplatform();

	cplatform_free(&cplatform);

	return ret;
}
