/* See UNLICENSE file for copyright and license details. */
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define MGD_IMPLEMENTATION
#include "minidraw.h"

#define BLACK 0
#define WHITE 15

static void
draw_centered_text(const struct draw *draw, const char *text, int size)
{
	/*
	 * since text is drawn in an 8x8 font, the text will be
	 * (8 * strlen(text) * size) pixels wide and (8 * size)
	 * pixels high.
	 *
	 * we take the middle of the window, and subtract HALF of
	 * the text's width and height to make it centered.
	 */

	/* draw text to memory buffer */
	mgd_fill(draw, WHITE); /* clear contents */
	mgd_drawtext(draw, BLACK,
		(int)draw->width / 2 - ((8 * (int)strlen(text) * size) / 2),
		(int)draw->height / 2 - (8 * size / 2), text, size);

	/* draw buffer contents to window */
	mg_draw(draw->data, draw->width, draw->height, draw->pixel_format, 0, 0);
	mg_flush(); /* make sure our changes are written to the screen */
}

int
main(void)
{
	struct mg_event event;
	jmp_buf env;
	struct draw draw = {
		.data = NULL,
		.pixel_format = MG_PIXEL_FORMAT_256
	};

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		free(draw.data); /* free(NULL) is a no-op */
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "hello world", env);

	/* create draw buffer */
	draw.width = (uint32_t)mg_width;
	draw.height = (uint32_t)mg_height;
	draw.data = malloc(draw.width * draw.height * sizeof(uint32_t));
	if (!draw.data) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	/* draw text */
	draw_centered_text(&draw, "hello world", 3);

	for (;;) {
		/* wait until an event is available */
		mg_waitevent(&event);
		if (event.type == MG_QUIT) {
			break;
		} else if (event.type == MG_RESIZE) {
			/* resize draw buffer */
			draw.width = (uint32_t)mg_width;
			draw.height = (uint32_t)mg_height;
			draw.data = realloc(draw.data, draw.width * draw.height * sizeof(uint32_t));
			if (!draw.data) {
				fputs("realloc: out of memory\n", stderr);
				return 1;
			}

			/* clear and redraw */
			draw_centered_text(&draw, "hello world", 3);
		} else if (event.type == MG_REDRAW) {
			/* clear and redraw */
			draw_centered_text(&draw, "hello world", 3);
		}
	}

	/* cleanup */
	free(draw.data);
	mg_quit();
	return 0;
}
