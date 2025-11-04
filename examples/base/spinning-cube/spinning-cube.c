/* See UNLICENSE file for copyright and license details. */
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define WIDTH 640
#define HEIGHT 480

#define CUBE_WIDTH 40.0

#define DISTANCE_FROM_CAM 100
#define HORIZONTAL_OFFSET -40.0
#define K1 40.0

#define INCREMENT 0.5

#define BLACK  0
#define RED    12
#define GREEN  10
#define BLUE   9
#define YELLOW 14
#define CYAN   11
#define WHITE  15

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

static inline double
calculate_x(double x, double y, double z, double a, double b, double c)
{
	return y * sin(a) * sin(b) * cos(c) - z * cos(a) * sin(b) * cos(c) +
		y * cos(a) * sin(c) + z * sin(a) * sin(c) + x * cos(b) * cos(c);
}

static inline double
calculate_y(double x, double y, double z, double a, double b, double c)
{
	return y * cos(a) * cos(c) + z * sin(a) * cos(c) -
		y * sin(a) * sin(b) * sin(c) + z * cos(a) * sin(b) * sin(c) -
		x * cos(b) * sin(c);
}

static inline double
calculate_z(double x, double y, double z, double a, double b)
{
	return z * cos(a) * cos(b) - y * sin(a) * cos(b) + x * sin(b);
}

static void
calculate_for_surface(double cube_x, double cube_y, double cube_z, double a, double b, double c,
		double *z_buf, uint8_t *draw, uint8_t color)
{
	double x = calculate_x(cube_x, cube_y, cube_z, a, b, c);
	double y = calculate_y(cube_x, cube_y, cube_z, a, b, c);
	double z = calculate_z(cube_x, cube_y, cube_z, a, b) + DISTANCE_FROM_CAM;

	double ooz = 1 / z;

	int xp = WIDTH / 2 + HORIZONTAL_OFFSET + K1 * ooz * x * 2;
	int yp = HEIGHT / 2 + K1 * ooz * y;

	int idx = xp + yp * WIDTH;
	if (idx >= 0 && idx < WIDTH * HEIGHT) {
		if (ooz > z_buf[idx]) {
			z_buf[idx] = ooz;
			draw[idx] = color;
		}
	}
}

int
main(void)
{
	struct mg_event event;
	jmp_buf env;

	double cube_x, cube_y;
	double a, b, c;
	double *z_buf;
	uint8_t *draw;

	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_errstring());
		mg_quit();
		return 1;
	}

	/* initialize variables here to avoid gcc warnings */
	a = b = c = 0.0;
	z_buf = malloc(WIDTH * HEIGHT * sizeof(double));
	if (!z_buf) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	draw = malloc(WIDTH * HEIGHT);
	if (!draw) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	mg_init(WIDTH, HEIGHT, "spinning-cube.c", env);

	memset(&event, 0, sizeof(struct mg_event));
	for (;;) {
		/* don't really need to memset(&event, 0) every iter */
		if (mg_getevent(&event) && (event.events & MG_QUIT))
			break;

		memset(draw, WHITE, WIDTH * HEIGHT);
		memset(z_buf, 0, WIDTH * HEIGHT * sizeof(double));

		for (cube_x = -CUBE_WIDTH; cube_x < CUBE_WIDTH; cube_x += INCREMENT) {
			for (cube_y = -CUBE_WIDTH; cube_y < CUBE_WIDTH; cube_y += INCREMENT) {
				calculate_for_surface(cube_x, cube_y, -CUBE_WIDTH, a, b, c,
						z_buf, draw, BLACK);
				calculate_for_surface(CUBE_WIDTH, cube_y, cube_x, a, b, c,
						z_buf, draw, RED);
				calculate_for_surface(-CUBE_WIDTH, cube_y, -cube_x, a, b, c,
						z_buf, draw, GREEN);
				calculate_for_surface(-cube_x, cube_y, CUBE_WIDTH, a, b, c,
						z_buf, draw, BLUE);
				calculate_for_surface(cube_x, -CUBE_WIDTH, -cube_y, a, b, c,
						z_buf, draw, YELLOW);
				calculate_for_surface(cube_x, CUBE_WIDTH, cube_y, a, b, c,
						z_buf, draw, CYAN);
			}
		}

		mg_draw((uint32_t *)draw, WIDTH, HEIGHT, MG_PIXEL_FORMAT_256, 0, 0);
		mg_flush();

		a += 0.05;
		b += 0.05;
		c += 0.01;
		msleep(20);
	}
	free(draw);
	free(z_buf);
	mg_quit();
	return 0;
}
