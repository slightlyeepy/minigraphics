/* See UNLICENSE file for copyright and license details. */
#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define BLACK 0
#define WHITE 15

struct draw_buffer {
	uint8_t *data;
	uint32_t width, height;
};

static void
redraw(const struct draw_buffer *draw)
{
	/* clear buffer with white */
	memset(draw->data, WHITE, draw->width * draw->height);

	/* draw a 200x200 black square */
	const int size = 200;
	int x = draw->width / 2 - size / 2;
	int y = draw->height / 2 - size / 2, end_y = draw->height / 2 + size / 2;
	for (; y < end_y; ++y)
		memset(draw->data + (y * draw->width + x), BLACK, size);

	/*
	 * draw buffer contents to window.
	 * the cast to (uint32_t *) is necessary when using this pixel format
	 */
	mg_draw((uint32_t *)draw->data, draw->width, draw->height, MG_PIXEL_FORMAT_256, 0, 0);
	mg_flush(); /* make sure our changes are written to the screen */
}

int
main(void)
{
	struct mg_event event;
	jmp_buf env;
	struct draw_buffer draw;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_errstring());
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "basic-program.c", env);

	/* create draw buffer */
	draw.width = mg_width;
	draw.height = mg_height;
	draw.data = malloc(draw.width * draw.height);
	if (!draw.data) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	/* draw everything */
	redraw(&draw);

	for (;;) {
		/* wait until an event is available */
		mg_waitevent(&event);
		if (event.type == MG_QUIT) {
			break;
		} else if (event.type == MG_RESIZE) {
			/* resize draw buffer */
			draw.width = mg_width;
			draw.height = mg_height;
			draw.data = realloc(draw.data, draw.width * draw.height);
			if (!draw.data) {
				fputs("realloc: out of memory\n", stderr);
				return 1;
			}

			/* clear and redraw */
			redraw(&draw);
		} else if (event.type == MG_REDRAW) {
			/* clear and redraw */
			redraw(&draw);
		}
	}

	/* cleanup */
	free(draw.data);
	mg_quit();
	return 0;
}
