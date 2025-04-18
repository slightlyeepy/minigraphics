/* See UNLICENSE file for copyright and license details. */
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MG_IMPLEMENTATION
#include "../../minigraphics.h"

#define WIDTH 640
#define HEIGHT 480

#define CUBE_WIDTH 40.0f

#define DISTANCE_FROM_CAM 100
#define HORIZONTAL_OFFSET -40.0f
#define K1 40.0f

#define INCREMENT 0.5f

static int
msleep(long ms)
{
	/* sleep for ms milliseconds. */
	struct timespec ts;
	int ret;

	if (ms < 0) {
		errno = EINVAL;
		return -1;
	}

	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;

	do {
		ret = nanosleep(&ts, &ts);
	} while (ret && errno == EINTR);

	return ret;
}

static inline float
calculate_x(float x, float y, float z, float a, float b, float c)
{
	return y * sinf(a) * sinf(b) * cosf(c) - z * cosf(a) * sinf(b) * cosf(c) +
		y * cosf(a) * sinf(c) + z * sinf(a) * sinf(c) + x * cosf(b) * cosf(c);
}

static inline float
calculate_y(float x, float y, float z, float a, float b, float c)
{
	return y * cosf(a) * cosf(c) + z * sinf(a) * cosf(c) -
		y * sinf(a) * sinf(b) * sinf(c) + z * cosf(a) * sinf(b) * sinf(c) -
		x * cosf(b) * sinf(c);
}

static inline float
calculate_z(float x, float y, float z, float a, float b)
{
	return z * cosf(a) * cosf(b) - y * sinf(a) * cosf(b) + x * sinf(b);
}

static void
calculate_for_surface(float cube_x, float cube_y, float cube_z, float a, float b, float c,
		float *z_buf, uint32_t *draw, uint32_t color)
{
	float x = calculate_x(cube_x, cube_y, cube_z, a, b, c);
	float y = calculate_y(cube_x, cube_y, cube_z, a, b, c);
	float z = calculate_z(cube_x, cube_y, cube_z, a, b) + DISTANCE_FROM_CAM;

	float ooz = 1 / z;

	int xp = (int)(WIDTH / 2 + HORIZONTAL_OFFSET + K1 * ooz * x * 2);
	int yp = (int)(HEIGHT / 2 + K1 * ooz * y);

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

	float cube_x, cube_y;
	float a, b, c;
	float *z_buf;
	uint32_t *draw;

	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* initialize variables here to avoid gcc warnings */
	a = b = c = 0.0f;
	z_buf = malloc(WIDTH * HEIGHT * sizeof(float));
	if (!z_buf) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	draw = malloc(WIDTH * HEIGHT * sizeof(uint32_t));
	if (!draw) {
		free(z_buf);
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	mg_init(WIDTH, HEIGHT, "cube", env);

	for (;;) {
		if (mg_getevent(&event) && event.type == MG_QUIT)
			break;

		memset(draw, 0xff, WIDTH * HEIGHT * sizeof(uint32_t));
		memset(z_buf, 0, WIDTH * HEIGHT * sizeof(float));

		for (cube_x = -CUBE_WIDTH; cube_x < CUBE_WIDTH; cube_x += INCREMENT) {
			for (cube_y = -CUBE_WIDTH; cube_y < CUBE_WIDTH; cube_y += INCREMENT) {
				calculate_for_surface(cube_x, cube_y, -CUBE_WIDTH, a, b, c,
						z_buf, draw, 0x00000000);
				calculate_for_surface(CUBE_WIDTH, cube_y, cube_x, a, b, c,
						z_buf, draw, 0x00ff0000);
				calculate_for_surface(-CUBE_WIDTH, cube_y, -cube_x, a, b, c,
						z_buf, draw, 0x0000ff00);
				calculate_for_surface(-cube_x, cube_y, CUBE_WIDTH, a, b, c,
						z_buf, draw, 0x000000ff);
				calculate_for_surface(cube_x, -CUBE_WIDTH, -cube_y, a, b, c,
						z_buf, draw, 0x00ffff00);
				calculate_for_surface(cube_x, CUBE_WIDTH, cube_y, a, b, c,
						z_buf, draw, 0x0000ffff);
			}
		}

		mg_draw(draw, WIDTH, HEIGHT, MG_PIXEL_FORMAT_XRGB, 0, 0);
		mg_flush();

		a += 0.05f;
		b += 0.05f;
		c += 0.01f;
		msleep(20);
	}
	free(draw);
	free(z_buf);
	mg_quit();
	return 0;
}
