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

#include "utahTeapot.c"

#define NSEC 1000000000

#define WIDTH 960
#define HEIGHT 720
#define BACKGROUND_COLOR 0xFF181818

#define PI 3.14159265359

typedef struct {
	double x, y;
} Vector2;

static Vector2
make_vector2(double x, double y)
{
	Vector2 v2;
	v2.x = x;
	v2.y = y;
	return v2;
}

typedef struct {
	double x, y, z;
} Vector3;

static Vector3
make_vector3(double x, double y, double z)
{
	Vector3 v3;
	v3.x = x;
	v3.y = y;
	v3.z = z;
	return v3;
}

#define EPSILON 1e-6

static Vector2
project_3d_2d(Vector3 v3)
{
	if (v3.z < 0) v3.z = -v3.z;
	if (v3.z < EPSILON) v3.z += EPSILON;
	return make_vector2(v3.x/v3.z, v3.y/v3.z);
}

static Vector2
project_2d_scr(Vector2 v2)
{
	return make_vector2((v2.x + 1)/2*WIDTH, (1 - (v2.y + 1)/2)*HEIGHT);
}

static Vector3
rotate_y(Vector3 p, double delta_angle)
{
	double angle = atan2(p.z, p.x) + delta_angle;
	double mag = sqrt(p.x*p.x + p.z*p.z);
	return make_vector3(cos(angle)*mag, p.y, sin(angle)*mag);
}

typedef enum {
	FACE_V1,
	FACE_V2,
	FACE_V3,
	FACE_VT1,
	FACE_VT2,
	FACE_VT3,
	FACE_VN1,
	FACE_VN2,
	FACE_VN3,
} Face_Index;

double
vector3_dot(Vector3 a, Vector3 b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

static void
redraw(double dt, uint32_t *data, double *zbuffer)
{
	static double angle = 0.0;
	angle += 0.25*PI*dt;

	Olivec_Canvas oc = olivec_canvas(data, WIDTH, HEIGHT, WIDTH);
	olivec_fill(oc, BACKGROUND_COLOR);
	for (size_t i = 0; i < WIDTH*HEIGHT; ++i)
		zbuffer[i] = 0;

	Vector3 camera = {0, 0, 1};
	for (size_t i = 0; i < faces_count; ++i) {
		int a, b, c;

		a = faces[i][FACE_V1];
		b = faces[i][FACE_V2];
		c = faces[i][FACE_V3];
		Vector3 v1 = rotate_y(make_vector3(vertices[a][0], vertices[a][1], vertices[a][2]), angle);
		Vector3 v2 = rotate_y(make_vector3(vertices[b][0], vertices[b][1], vertices[b][2]), angle);
		Vector3 v3 = rotate_y(make_vector3(vertices[c][0], vertices[c][1], vertices[c][2]), angle);
		v1.z += 1.5; v2.z += 1.5; v3.z += 1.5;

		a = faces[i][FACE_VN1];
		b = faces[i][FACE_VN2];
		c = faces[i][FACE_VN3];
		Vector3 vn1 = rotate_y(make_vector3(normals[a][0], normals[a][1], normals[a][2]), angle);
		Vector3 vn2 = rotate_y(make_vector3(normals[b][0], normals[b][1], normals[b][2]), angle);
		Vector3 vn3 = rotate_y(make_vector3(normals[c][0], normals[c][1], normals[c][2]), angle);
		if (vector3_dot(camera, vn1) > 0.0 &&
			vector3_dot(camera, vn2) > 0.0 &&
			vector3_dot(camera, vn3) > 0.0)
			continue;


		Vector2 p1 = project_2d_scr(project_3d_2d(v1));
		Vector2 p2 = project_2d_scr(project_3d_2d(v2));
		Vector2 p3 = project_2d_scr(project_3d_2d(v3));

		int x1 = p1.x;
		int x2 = p2.x;
		int x3 = p3.x;
		int y1 = p1.y;
		int y2 = p2.y;
		int y3 = p3.y;
		int lx, hx, ly, hy;
		if (olivec_normalize_triangle(oc.width, oc.height, x1, y1, x2, y2, x3, y3, &lx, &hx, &ly, &hy)) {
			for (int y = ly; y <= hy; ++y) {
				for (int x = lx; x <= hx; ++x) {
					int u1, u2, det;
					if (olivec_barycentric(x1, y1, x2, y2, x3, y3, x, y, &u1, &u2, &det)) {
						int u3 = det - u1 - u2;
						double z = 1/v1.z*u1/det + 1/v2.z*u2/det + 1/v3.z*u3/det;
						double near = 0.1;
						double far = 5.0;
						if (1.0/far < z && z < 1.0/near && z > zbuffer[y*WIDTH + x]) {
							zbuffer[y*WIDTH + x] = z;
							OLIVEC_PIXEL(oc, x, y) = mix_colors3(0xFF1818FF, 0xFF18FF18, 0xFFFF1818, u1, u2, det);

							z = 1.0/z;
							if (z >= 1.0) {
								z -= 1.0;
								uint32_t v = z*255;
								if (v > 255)
									v = 255;
								olivec_blend_color(&OLIVEC_PIXEL(oc, x, y), (v<<(3*8)));
							}
						}
					}
				}
			}
		}
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
	double *zbuf;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a window */
	mg_init(WIDTH, HEIGHT, "TEMPLATE", env);

	/* create  buffers */
	data = malloc(WIDTH * HEIGHT * sizeof(uint32_t));
	zbuf = malloc(WIDTH * HEIGHT * sizeof(double));
	if (!data || !zbuf) {
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
		redraw(deltatime, data, zbuf);

		/* at least 10ms delay between frames */
		msleep(10);

		first = false;
	}

	/* cleanup */
	free(zbuf);
	free(data);
	mg_quit();
	return 0;
}
