/* See UNLICENSE file for copyright and license details. */
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#define MG_IMPLEMENTATION
#include "../../minigraphics.h"

static void
draw_centered_text(const char *text, int size)
{
	/*
	 * since text is drawn in an 8x8 font, the text will be
	 * (8 * strlen(text) * size) pixels wide and (8 * size)
	 * pixels high.
	 *
	 * we take the middle of the window, and subtract HALF of
	 * the text's width and height to make it centered.
	 */
	mg_drawtext(mg_width / 2 - ((8 * (int)strlen(text) * size) / 2),
			mg_height / 2 - (8 * size / 2), text, size);
	mg_flush(); /* make sure our changes are written to the screen */
}

int
main(void)
{
	struct mg_event event;
	jmp_buf env;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "hello world", env);

	/* draw text */
	draw_centered_text("hello world", 3);

	for (;;) {
		/* wait until an event is available */
		mg_waitevent(&event);
		if (event.type == MG_QUIT) {
			break;
		} else if (event.type == MG_RESIZE || event.type == MG_REDRAW) {
			/* clear and redraw */
			mg_clear();
			draw_centered_text("hello world", 3);
		}
	}

	/* cleanup */
	mg_quit();
	return 0;
}
