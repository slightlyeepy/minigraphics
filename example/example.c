/* See UNLICENSE file for copyright and license details. */
#define _POSIX_C_SOURCE 199309L
#include <errno.h>
#include <setjmp.h>
#include <stdio.h>
#include <time.h>

#define MG_IMPLEMENTATION
#include "../minigraphics.h"

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
	int xpos, ypos, cont;

	jmp_buf env;
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* initialize here to avoid gcc warnings */
	xpos = 0;
	ypos = 200;
	cont = 1;

	/*
	 * create a 640x480 window.
	 *
	 * note that the actual size might be different sometimes
	 * (e.g when using a tiling window manager), so
	 * the window dimensions are stored in the mg_width (width) and
	 * mg_height (height) variables.
	 */
	mg_init(640, 480, "example mg program", env);
	mg_setbgcolor(255, 255, 255);
	mg_setdrawcolor(0, 0, 0);

	while (cont) {
		if (mg_getevent(&event)) {
			switch (event.type) {
			case MG_QUIT:
				cont = 0;
				break;
			case MG_RESIZE:
				printf("window resized: width = %d height = %d\n", mg_width, mg_height);
				break;
			case MG_KEYDOWN:
				/* note: lower Y position = higher */
				if (event.key == XKB_KEY_w || event.key == XKB_KEY_W) {
					/* if W is pressed, move square up. */
					if (ypos < 50) {
						ypos = mg_height;
					} else {
						ypos -= 50;
					}
				} else if (event.key == XKB_KEY_s || event.key == XKB_KEY_S) {
					/* if S is pressed, move square down. */
					if (ypos > mg_height - 50) {
						ypos = 0;
					} else {
						ypos += 50;
					}
				}
				printf("key %u pressed\n", event.key);
				break;
			case MG_KEYUP:
				printf("key %u released\n", event.key);
				break;
			case MG_MOUSEDOWN:
				printf("mouse clicked: button = %u\n", event.button);
				break;
			case MG_MOUSEUP:
				printf("mouse up: button = %u\n", event.button);
				break;
			case MG_MOUSEMOTION:
				printf("mouse motion: x = %d y = %d\n", event.x, event.y);
				break;
			default:
				break;
			}
		}

		if (xpos == mg_width)
			/* wrap to avoid going off-screen */
			xpos = 0;
		else
			++xpos;
		mg_clear();

		mg_fillrect(xpos + 50, ypos + 50, xpos + 100, ypos + 100);
		mg_drawrect(xpos + 200, ypos, xpos + 225, ypos + 25);
		mg_fillcircle(xpos, ypos + 200, 100);
		mg_drawcircle(xpos + 200, ypos + 200, 100);
		mg_drawrect(xpos + 100, ypos + 100, xpos + 300, ypos + 300);
		mg_filltriangle(xpos, ypos - 100, xpos + 50, ypos, xpos - 50, ypos);
		mg_filltriangle(100, 100, 130, 200, 80, 300);
		mg_drawtext(xpos + 100, 100, "hello world", 3);

		mg_flush(); /* make sure our changes are written to the screen */

		msleep(50); /* 20 FPS */
	}
	mg_quit();
	return 0;
}
