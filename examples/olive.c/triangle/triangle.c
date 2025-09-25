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

#define WIDTH 960
#define HEIGHT 720
#define BACKGROUND_COLOR 0xFF181818
#define CIRCLE_RADIUS 100
#define CIRCLE_COLOR 0x99AA2020

#define PI 3.14159265359

static inline void
rotate_point(double *x, double *y, double triangle_angle)
{
	double dx = *x - WIDTH/2;
	double dy = *y - HEIGHT/2;
	double mag = sqrt(dx*dx + dy*dy);
	double dir = atan2(dy, dx) + triangle_angle;
	*x = cos(dir)*mag + WIDTH/2;
	*y = sin(dir)*mag + HEIGHT/2;
}

static void
redraw(double dt, uint32_t *data)
{
	static double triangle_angle = 0;
	static double circle_x = WIDTH/2;
	static double circle_y = HEIGHT/2;
	static double circle_dx = 100;
	static double circle_dy = 100;

	Olivec_Canvas oc = olivec_canvas(data, WIDTH, HEIGHT, WIDTH);
	olivec_fill(oc, BACKGROUND_COLOR);

	// Triangle
	{
		triangle_angle += 0.5*PI*dt;

		double x1 = WIDTH/2, y1 = HEIGHT/8;
		double x2 = WIDTH/8, y2 = HEIGHT/2;
		double x3 = WIDTH*7/8, y3 = HEIGHT*7/8;
		rotate_point(&x1, &y1, triangle_angle);
		rotate_point(&x2, &y2, triangle_angle);
		rotate_point(&x3, &y3, triangle_angle);
		olivec_triangle3c(oc, x1, y1, x2, y2, x3, y3, 0xFF2020FF, 0xFF20FF20, 0xFFFF2020);
	}

	// Circle
	{
		double x = circle_x + circle_dx*dt;
		if (x - CIRCLE_RADIUS < 0 || x + CIRCLE_RADIUS >= WIDTH)
			circle_dx *= -1;
		else
			circle_x = x;

		double y = circle_y + circle_dy*dt;
		if (y - CIRCLE_RADIUS < 0 || y + CIRCLE_RADIUS >= HEIGHT)
			circle_dy *= -1;
		else
			circle_y = y;

		olivec_circle(oc, circle_x, circle_y, CIRCLE_RADIUS, CIRCLE_COLOR);
	}

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
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a window */
	mg_init(WIDTH, HEIGHT, "triangle.c", env);

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
