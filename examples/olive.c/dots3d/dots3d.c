/* See LICENSE file for copyright and license details. */
#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define OLIVEC_IMPLEMENTATION
#include "olive.c"

#define NSEC 1000000000

#define PI 3.14159265359

#define WIDTH 960
#define HEIGHT 720

#define BACKGROUND_COLOR 0xff181818
#define GRID_COUNT 10
#define GRID_PAD 0.5/GRID_COUNT
#define GRID_SIZE ((GRID_COUNT - 1)*GRID_PAD)
#define CIRCLE_RADIUS 5
#define Z_START 0.25
#define ABOBA_PADDING 50

static void
redraw(double dt, uint32_t *data)
{
	static double angle = 0.0;
	angle += 0.25*PI*dt;

	Olivec_Canvas oc = olivec_canvas(data, WIDTH, HEIGHT, WIDTH);

	olivec_fill(oc, BACKGROUND_COLOR);
	for (int ix = 0; ix < GRID_COUNT; ++ix) {
		for (int iy = 0; iy < GRID_COUNT; ++iy) {
			for (int iz = 0; iz < GRID_COUNT; ++iz) {
				double x = ix*GRID_PAD - GRID_SIZE/2;
				double y = iy*GRID_PAD - GRID_SIZE/2;
				double z = Z_START + iz*GRID_PAD;

				double cx = 0.0;
				double cz = Z_START + GRID_SIZE/2;

				double dx = x - cx;
				double dz = z - cz;

				double a = atan2(dz, dx);
				double m = sqrt(dx*dx + dz*dz);

				dx = cosf(a + angle)*m;
				dz = sinf(a + angle)*m;

				x = dx + cx;
				z = dz + cz;

				x /= z;
				y /= z;

				uint32_t r = ix*255/GRID_COUNT;
				uint32_t g = iy*255/GRID_COUNT;
				uint32_t b = iz*255/GRID_COUNT;
				uint32_t color = 0xff000000 | (r<<(0*8)) | (g<<(1*8)) | (b<<(2*8));
				olivec_circle(oc, (x + 1)/2*WIDTH, (y + 1)/2*HEIGHT, CIRCLE_RADIUS, color);
			}
		}
	}

	const size_t size = 8;
	olivec_text(oc, "aboba", ABOBA_PADDING, HEIGHT - ABOBA_PADDING - olivec_default_font.height*size, olivec_default_font, size, 0xffffffff);

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
		mg_quit();
		return 1;
	}

	/* create a window */
	mg_init(WIDTH, HEIGHT, "dots3d.c", env);

	/* create draw buffer */
	data = malloc(WIDTH * HEIGHT * sizeof(uint32_t));
	if (!data) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	struct timespec tp_prev, tp;
	double deltatime = 0.0;
	bool first = true;
	memset(&event, 0, sizeof(struct mg_event));
	for (;;) {
		/* don't really need to memset(&event, 0) every iter */
		if (mg_getevent(&event) && (event.events & MG_QUIT))
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
