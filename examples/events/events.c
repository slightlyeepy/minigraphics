/* See UNLICENSE file for copyright and license details. */
#include <setjmp.h>
#include <stdio.h>
#include <string.h>

#include <xkbcommon/xkbcommon.h>

#define MG_IMPLEMENTATION
#include "minigraphics.h"

static void
print_key(const struct mg_event *event)
{
	char buf[128];
	fputs(event->type == MG_KEYDOWN ? "recieved event: MG_KEYDOWN:     " :
			"recieved event: MG_KEYUP:       ", stdout);
	if (xkb_keysym_get_name(event->key, buf, sizeof(buf)) < 0)
		puts("invalid keysym");
	else
		puts(buf);
}

static void
print_mousebtn(const struct mg_event *event)
{
	static const char *buttonnames[] = {
		"no button",
		"left mouse button",
		"middle mouse button",
		"right mouse button",
		"side mouse button",
		"extra mouse button"
	};

	printf(event->type == MG_MOUSEDOWN ? "recieved event: MG_MOUSEDOWN:   %s\n" :
			"recieved event: MG_MOUSEUP:     %s\n", buttonnames[event->button]);
}

int
main(void)
{
	struct mg_event event;
	jmp_buf env;
	int cont = 1;

	/* if a library error happens, a longjmp() to here will happen. */
	if (setjmp(env)) {
		fprintf(stderr, "mg error: %s\n", mg_strerror(mg_errno));
		return 1;
	}

	/* create a 640x480 window */
	mg_init(640, 480, "events", env);

	while (cont) {
		mg_waitevent(&event);
		switch (event.type) {
		case MG_QUIT:
			puts("recieved event: MG_QUIT");
			cont = 0;
			break;
		case MG_RESIZE:
			printf("recieved event: MG_RESIZE:      width = %d, height = %d\n", mg_width, mg_height);
			break;
		case MG_REDRAW:
			puts("recieved event: MG_REDRAW");
			break;
		case MG_KEYDOWN: /* FALLTHROUGH */
		case MG_KEYUP:
			print_key(&event);
			break;
		case MG_MOUSEDOWN: /* FALLTHROUGH */
		case MG_MOUSEUP:
			print_mousebtn(&event);
			break;
		case MG_MOUSEMOTION:
			printf("recieved event: MG_MOUSEMOTION: x = %d, y = %d\n", event.x, event.y);
			break;
		default:
			break;
		}
			
	}

	/* cleanup */
	mg_quit();
	return 0;
}
