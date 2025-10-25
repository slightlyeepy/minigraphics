/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#define NSEC 1000000000

static void
redraw(double dt, uint32_t *data)
{

	/* draw buffer contents to window */
	mg_draw(data, WIDTH, HEIGHT, MG_PIXEL_FORMAT_XBGR, 0, 0);
	mg_flush(); /* make sure our changes are written to the screen */
}

static void
msleep(long ms)
{
	/* sleep for ms milliseconds. */
	struct timespec tp;
	int ret;

	tp.tv_sec = ms / 1000;
	tp.tv_nsec = (ms % 1000) * 1000000;

	do {
		ret = nanosleep(&tp, &tp);
	} while (ret && errno == EINTR);

	/* don't error-check any further -- nanosleep() can only fail on signal interruption or invalid argument */
}

int
main(void)
{
	struct mg_event event;
	jmp_buf env;
	uint32_t *data;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_errstring());
		return 1;
	}

	/* create a window */
	mg_init(WIDTH, HEIGHT, "TEMPLATE", env);

	/* create draw buffer */
	data = malloc(WIDTH * HEIGHT * sizeof(uint32_t));
	if (!data) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	struct timespec tp_prev, tp;
	double deltatime = 0.0;
	bool first = true;
	for (;;) {
		if (mg_getevent(&event) && event.type == MG_QUIT)
			break;

		/* get deltatime */
		memcpy(&tp_prev, &tp, sizeof(struct timespec));
		if (clock_gettime(CLOCK_MONOTONIC, &tp) < 0) {
			perror("clock_gettime");
			return 1;
		}
		if (!first)
			deltatime = (tp.tv_sec - tp_prev.tv_sec) + ((double)tp.tv_nsec / NSEC - (double)tp_prev.tv_nsec / NSEC);

		/* draw everything */
		redraw(deltatime, data);

		/* at least 10ms delay between frames */
		msleep(10);

		first = false;
	}

	/* cleanup */
	free(data);
	mg_quit();
	return 0;
}
