/* See UNLICENSE file for copyright and license details. */
#include <arpa/inet.h>

#include <errno.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

#define LEN(x) (sizeof(x) / sizeof(x[0]))
#define FCLOSE_IF_NOT_STDIN(f) \
	if (f != stdin) \
		fclose(f)

static const char *argv0;

static void
die(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (fmt) {
		fprintf(stderr, "%s: ", argv0);
		vfprintf(stderr, fmt, ap);
		if (fmt[0] && fmt[strlen(fmt) - 1] == ':')
			fprintf(stderr, " %s\n", strerror(errno));
		else
			putc('\n', stderr);
	}
	va_end(ap);
	exit(1);
}

static void
read_farbfeld(FILE *f, uint32_t **data, uint32_t *width, uint32_t *height)
{
	uint32_t header[4], w, h;
	uint16_t rgba[4];
	size_t i = 0, j = 0;

	if (fread(header, sizeof(header[0]), LEN(header), f) != LEN(header)) {
		FCLOSE_IF_NOT_STDIN(f);
		die("read error:");
	}

	if (memcmp("farbfeld", header, sizeof("farbfeld") - 1)) {
		FCLOSE_IF_NOT_STDIN(f);
		die("error: invalid magic value");
	}

	w = ntohl(header[2]);
	h = ntohl(header[3]);

	*data = malloc(w * h * sizeof(uint32_t));
	if (!(*data)) {
		FCLOSE_IF_NOT_STDIN(f);
		die("error: out of memory");
	}

	for (i = 0; i < w * h; ++i) {
		if (fread(rgba, sizeof(rgba[0]), LEN(rgba), f) != LEN(rgba)) {
			FCLOSE_IF_NOT_STDIN(f);
			die("read error:");
		}

		/* note: this isn't really proper color reduction... */
		(*data)[j++] = ((uint8_t)ceilf((float)(ntohs(rgba[0])) / 257) << 24) |
			((uint8_t)ceilf((float)(ntohs(rgba[1])) / 257) << 16) |
			((uint8_t)ceilf((float)(ntohs(rgba[2])) / 257) << 8);
	}
	*width = w;
	*height = h;
	fclose(f);
}

int
main(int argc, char *argv[])
{
	FILE *image_source;
	struct mg_event event;
	uint32_t *img_data;
	uint32_t img_width, img_height;
	jmp_buf env;

	argv0 = argv[0];

	if (argc < 2) {
		fprintf(stderr, "usage: %s image\n"
				"image must be a file in farbfeld format; pass - to read from stdin\n",
				argv[0]);
	}

	if (!strcmp(argv[1], "-")) {
		image_source = stdin;
	} else {
		image_source = fopen(argv[1], "r");
		if (!image_source)
			die("failed to open file '%s':", argv[1]);
	}

	read_farbfeld(image_source, &img_data, &img_width, &img_height);

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env))
		die("mg error: %s", mg_strerror(mg_errno));

	/* create a window of the same size as the image */
	mg_init((int)img_width, (int)img_height, "image viewer", env);

	/* display image */
	mg_draw(img_data, img_width, img_height, MG_PIXEL_FORMAT_RGBX, 0, 0);
	mg_flush(); /* make sure our changes are written to the screen */

	for (;;) {
		/* wait until an event is available */
		mg_waitevent(&event);
		if (event.type == MG_QUIT) {
			break;
		} else if (event.type == MG_RESIZE || event.type == MG_REDRAW) {
			/* clear and redraw */
			mg_clear();
			mg_draw(img_data, img_width, img_height, MG_PIXEL_FORMAT_RGBX, 0, 0);
			mg_flush(); /* make sure our changes are written to the screen */
		}
	}

	/* cleanup */
	free(img_data);
	mg_quit();
	return 0;
}
