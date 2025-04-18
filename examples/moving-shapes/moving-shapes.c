/* See UNLICENSE file for copyright and license details. */
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define MGD_IMPLEMENTATION
#include "util/minidraw.h"

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

int
main(void)
{
	struct mg_event event;
	jmp_buf env;
	uint32_t *draw = NULL;
	uint32_t w, h;

	int xpos, ypos;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "moving shapes", env);

	/* create draw buffer */
	w = (uint32_t)mg_width;
	h = (uint32_t)mg_height;
	draw = malloc(w * h * sizeof(uint32_t));
	if (!draw) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	xpos = 0;
	ypos = 200;

	for (;;) {
		/* check if an event is available */
		if (mg_getevent(&event)) {
			if (event.type == MG_QUIT) {
				break;
			} else if (event.type == MG_RESIZE) {
				/* resize draw buffer */
				w = (uint32_t)mg_width;
				h = (uint32_t)mg_height;
				draw = realloc(draw, w * h * sizeof(uint32_t));
				if (!draw) {
					fputs("realloc: out of memory\n", stderr);
					return 1;
				}
			} else if (event.type == MG_KEYDOWN) {
				/* note: lower Y position = higher */
				if (event.key == XKB_KEY_w || event.key == XKB_KEY_W) {
					/* if W is pressed, move square up. */
					if (ypos < 50)
						ypos = mg_height;
					else
						ypos -= 50;
				} else if (event.key == XKB_KEY_s || event.key == XKB_KEY_S) {
					/* if S is pressed, move square down. */
					if (ypos > mg_height - 50)
						ypos = 0;
					else
						ypos += 50;
				}
			}
		}

		if (xpos == mg_width)
			/* wrap to avoid going off-screen */
			xpos = 0;
		else
			++xpos;

		mgd_fill(draw, w, h, 0x00ffffff); /* clear contents */

		/* draw a bunch of shapes */
		mgd_fillrect(draw, w, h, 0x00000000, xpos + 50, ypos + 50, xpos + 100, ypos + 100);
		mgd_drawrect(draw, w, h, 0x00000000, xpos + 200, ypos, xpos + 225, ypos + 25);
		mgd_fillcircle(draw, w, h, 0x00000000, xpos, ypos + 200, 100);
		mgd_drawcircle(draw, w, h, 0x00000000, xpos + 200, ypos + 200, 100);
		mgd_drawrect(draw, w, h, 0x00000000, xpos + 100, ypos + 100, xpos + 300, ypos + 300);
		mgd_filltriangle(draw, w, h, 0x00000000, xpos, ypos - 100, xpos + 50, ypos, xpos - 50, ypos);
		mgd_filltriangle(draw, w, h, 0x00000000, xpos + 200, ypos + 300, xpos + 230, ypos + 400,
				xpos + 180, ypos + 500);

		/* 264 = width of text divided by 2 */
		mgd_drawtext(draw, w, h, 0x00000000, mg_width / 2 - 264, 100, "use W or S to move shapes up/down", 2);

		/* draw buffer contents to window */
		mg_draw(draw, w, h, MG_PIXEL_FORMAT_XRGB, 0, 0);
		mg_flush(); /* make sure our changes are written to the screen */

		msleep(50); /* 20 FPS */
	}

	/* cleanup */
	mg_quit();
	return 0;
}
