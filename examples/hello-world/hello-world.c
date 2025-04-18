/* See UNLICENSE file for copyright and license details. */
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define MGD_IMPLEMENTATION
#include "util/minidraw.h"

static void
draw_centered_text(uint32_t *draw, uint32_t w, uint32_t h,
		const char *text, int size)
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
	mgd_fill(draw, w, h, 0x00ffffff); /* clear contents */
	mgd_drawtext(draw, w, h, 0x00000000,
			(int)w / 2 - ((8 * (int)strlen(text) * size) / 2),
			(int)h / 2 - (8 * size / 2), text, size);

	/* draw buffer contents to window */
	mg_draw(draw, w, h, MG_PIXEL_FORMAT_XRGB, 0, 0);
	mg_flush(); /* make sure our changes are written to the screen */
}

int
main(void)
{
	struct mg_event event;
	jmp_buf env;
	uint32_t *draw = NULL;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		free(draw); /* free(NULL) is a no-op */
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "hello world", env);

	/* create draw buffer */
	draw = malloc((uint32_t)mg_width * (uint32_t)mg_height *
			sizeof(uint32_t));
	if (!draw) {
		fputs("malloc: out of memory\n", stderr);
		return 1;
	}

	/* draw text */
	draw_centered_text(draw, (uint32_t)mg_width, (uint32_t)mg_height,
			"hello world", 3);

	for (;;) {
		/* wait until an event is available */
		mg_waitevent(&event);
		if (event.type == MG_QUIT) {
			break;
		} else if (event.type == MG_RESIZE) {
			/* resize draw buffer */
			draw = realloc(draw, (uint32_t)mg_width *
					(uint32_t)mg_height * sizeof(uint32_t));
			if (!draw) {
				fputs("realloc: out of memory\n", stderr);
				return 1;
			}

			/* clear and redraw */
			draw_centered_text(draw, (uint32_t)mg_width,
					(uint32_t)mg_height, "hello world", 3);
		} else if (event.type == MG_REDRAW) {
			/* clear and redraw */
			draw_centered_text(draw, (uint32_t)mg_width,
					(uint32_t)mg_height, "hello world", 3);
		}
	}

	/* cleanup */
	free(draw);
	mg_quit();
	return 0;
}
