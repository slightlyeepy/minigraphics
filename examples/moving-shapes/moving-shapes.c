/* See UNLICENSE file for copyright and license details. */
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MG_IMPLEMENTATION
#include "../../minigraphics.h"

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
	int xpos, ypos;
	jmp_buf env;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "moving shapes", env);

	xpos = 0;
	ypos = 200;

	for (;;) {
		/* check if an event is available */
		if (mg_getevent(&event)) {
			if (event.type == MG_QUIT) {
				break;
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

		mg_clear();

		/* draw a bunch of shapes */
		mg_fillrect(xpos + 50, ypos + 50, xpos + 100, ypos + 100);
		mg_drawrect(xpos + 200, ypos, xpos + 225, ypos + 25);
		mg_fillcircle(xpos, ypos + 200, 100);
		mg_drawcircle(xpos + 200, ypos + 200, 100);
		mg_drawrect(xpos + 100, ypos + 100, xpos + 300, ypos + 300);
		mg_filltriangle(xpos, ypos - 100, xpos + 50, ypos, xpos - 50, ypos);
		mg_filltriangle(xpos + 200, ypos + 300, xpos + 230, ypos + 400,
				xpos + 180, ypos + 500);

		/* 264 = width of text divided by 2 */
		mg_drawtext(mg_width / 2 - 264, 100, "use W or S to move shapes up/down", 2);

		mg_flush(); /* make sure our changes are written to the screen */

		msleep(50); /* 20 FPS */
	}

	/* cleanup */
	mg_quit();
	return 0;
}
