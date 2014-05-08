/*
 * test_init.c
 *
 * Test for Red Hat bug #1095366 - double free or corruption with multiple
 * log4c_init and log4c_fini
 *
 * Doing log4c_init()/log4c_fini() calls would be enough for testing the bug,
 * here is also initialized a stream appender.
 *
 * This test opens /dev/null.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include <log4c.h>
#include <sd/test.h>

static log4c_category_t* root = NULL;
static log4c_appender_t* appender = NULL;

static int test_init(sd_test_t* a_test, int argc, char *argv[]) {
	return 1;
}

int main(int argc, char *argv[]) {
	sd_test_t *t;
	FILE *f;
	int ret = 0, i;

	t = sd_test_new(argc, argv);
	sd_test_add(t, test_init);

	for (i = 0; i < 100; i++) {
		log4c_init();

		appender = log4c_appender_get("stream");
		f = fopen("/dev/null", "w+");
		log4c_appender_set_udata(appender, f);

		root = log4c_category_get("root");
		log4c_category_set_appender(root, appender);
		log4c_category_set_priority(root, LOG4C_PRIORITY_TRACE);

		if (i == 0) {
			ret = sd_test_run(t, argc, argv);
		}

		log4c_appender_set_udata(appender, NULL);
		fclose(f);

		log4c_fini();
	}

	sd_test_delete(t);

	return !ret;
}
