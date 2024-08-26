/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * For more information, please refer to <http://unlicense.org/>
 */

/*
 * ===========================================================================
 * header
 */
#if !defined(MG_H)
#define MG_H
#include <stdint.h>

#include <xkbcommon/xkbcommon-keysyms.h>

extern int mg_width;
extern int mg_height;

/* types */
enum mg_event_type {
	MG_NOEVENT,
	MG_QUIT,
	MG_RESIZE,
	MG_REDRAW,
	MG_KEYDOWN,
	MG_KEYUP,
	MG_MOUSEDOWN,
	MG_MOUSEUP,
	MG_MOUSEMOTION
};

struct mg_event {
	enum mg_event_type type;

	/* both xkb_keysym_t and Xlib KeySym are uint32_t so this works */
	uint32_t key;

	int button;
	int x;
	int y;
};

/* drawing functions */
void mg_clear(void);
void mg_drawcircle(int x, int y, int r);
void mg_drawline(int x1, int y1, int x2, int y2);
void mg_drawpixel(int x, int y);
void mg_drawrect(int x1, int y1, int x2, int y2);
void mg_drawtext(int x, int y, const char *text, int size);
void mg_drawtriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void mg_fillcircle(int x, int y, int r);
void mg_fillrect(int x1, int y1, int x2, int y2);
void mg_filltriangle(int x1, int y1, int x2, int y2, int x3, int y3);
void mg_flush(void);

/* functions to change options */
void mg_setbgcolor(uint8_t r, uint8_t g, uint8_t b);
void mg_setdrawcolor(uint8_t r, uint8_t g, uint8_t b);

/* events */
void mg_waitevent(struct mg_event *event);

/* initialization & shutdown */
int mg_init(int w, int h, const char *title);
void mg_quit(void);
#endif /* !defined(MG_H) */

/*
 * ===========================================================================
 * implementation
 */
#if defined(MG_IMPLEMENTATION)

#if !defined(MG_BACKEND_X11) && !defined(MG_BACKEND_WAYLAND)
#error please define a backend for minigraphics.
#elif defined(MG_BACKEND_X11) && defined(MG_BACKEND_WAYLAND)
#error only one backend may be defined.
#endif

/*
 * ===========================================================================
 * font
 */
static const unsigned char mg_font[95][8] = {
	/* make sure that (c >= 0x20 && c <= 0x7e) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* U+0020 (space) */
	{ 0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00}, /* U+0021 (!) */
	{ 0x36, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* U+0022 (") */
	{ 0x36, 0x36, 0x7F, 0x36, 0x7F, 0x36, 0x36, 0x00}, /* U+0023 (#) */
	{ 0x0C, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x0C, 0x00}, /* U+0024 ($) */
	{ 0x00, 0x63, 0x33, 0x18, 0x0C, 0x66, 0x63, 0x00}, /* U+0025 (%) */
	{ 0x1C, 0x36, 0x1C, 0x6E, 0x3B, 0x33, 0x6E, 0x00}, /* U+0026 (&) */
	{ 0x06, 0x06, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00}, /* U+0027 (') */
	{ 0x18, 0x0C, 0x06, 0x06, 0x06, 0x0C, 0x18, 0x00}, /* U+0028 (() */
	{ 0x06, 0x0C, 0x18, 0x18, 0x18, 0x0C, 0x06, 0x00}, /* U+0029 ()) */
	{ 0x00, 0x66, 0x3C, 0xFF, 0x3C, 0x66, 0x00, 0x00}, /* U+002A (*) */
	{ 0x00, 0x0C, 0x0C, 0x3F, 0x0C, 0x0C, 0x00, 0x00}, /* U+002B (+) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x06}, /* U+002C (,) */
	{ 0x00, 0x00, 0x00, 0x3F, 0x00, 0x00, 0x00, 0x00}, /* U+002D (-) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C, 0x00}, /* U+002E (.) */
	{ 0x60, 0x30, 0x18, 0x0C, 0x06, 0x03, 0x01, 0x00}, /* U+002F (/) */
	{ 0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, /* U+0030 (0) */
	{ 0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, /* U+0031 (1) */
	{ 0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, /* U+0032 (2) */
	{ 0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, /* U+0033 (3) */
	{ 0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, /* U+0034 (4) */
	{ 0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, /* U+0035 (5) */
	{ 0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, /* U+0036 (6) */
	{ 0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, /* U+0037 (7) */
	{ 0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, /* U+0038 (8) */
	{ 0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, /* U+0039 (9) */
	{ 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, /* U+003A (:) */
	{ 0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x06}, /* U+003B (;) */
	{ 0x18, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x18, 0x00}, /* U+003C (<) */
	{ 0x00, 0x00, 0x3F, 0x00, 0x00, 0x3F, 0x00, 0x00}, /* U+003D (=) */
	{ 0x06, 0x0C, 0x18, 0x30, 0x18, 0x0C, 0x06, 0x00}, /* U+003E (>) */
	{ 0x1E, 0x33, 0x30, 0x18, 0x0C, 0x00, 0x0C, 0x00}, /* U+003F (?) */
	{ 0x3E, 0x63, 0x7B, 0x7B, 0x7B, 0x03, 0x1E, 0x00}, /* U+0040 (@) */
	{ 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}, /* U+0041 (A) */
	{ 0x3F, 0x66, 0x66, 0x3E, 0x66, 0x66, 0x3F, 0x00}, /* U+0042 (B) */
	{ 0x3C, 0x66, 0x03, 0x03, 0x03, 0x66, 0x3C, 0x00}, /* U+0043 (C) */
	{ 0x1F, 0x36, 0x66, 0x66, 0x66, 0x36, 0x1F, 0x00}, /* U+0044 (D) */
	{ 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x46, 0x7F, 0x00}, /* U+0045 (E) */
	{ 0x7F, 0x46, 0x16, 0x1E, 0x16, 0x06, 0x0F, 0x00}, /* U+0046 (F) */
	{ 0x3C, 0x66, 0x03, 0x03, 0x73, 0x66, 0x7C, 0x00}, /* U+0047 (G) */
	{ 0x33, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x33, 0x00}, /* U+0048 (H) */
	{ 0x1E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, /* U+0049 (I) */
	{ 0x78, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E, 0x00}, /* U+004A (J) */
	{ 0x67, 0x66, 0x36, 0x1E, 0x36, 0x66, 0x67, 0x00}, /* U+004B (K) */
	{ 0x0F, 0x06, 0x06, 0x06, 0x46, 0x66, 0x7F, 0x00}, /* U+004C (L) */
	{ 0x63, 0x77, 0x7F, 0x7F, 0x6B, 0x63, 0x63, 0x00}, /* U+004D (M) */
	{ 0x63, 0x67, 0x6F, 0x7B, 0x73, 0x63, 0x63, 0x00}, /* U+004E (N) */
	{ 0x1C, 0x36, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00}, /* U+004F (O) */
	{ 0x3F, 0x66, 0x66, 0x3E, 0x06, 0x06, 0x0F, 0x00}, /* U+0050 (P) */
	{ 0x1E, 0x33, 0x33, 0x33, 0x3B, 0x1E, 0x38, 0x00}, /* U+0051 (Q) */
	{ 0x3F, 0x66, 0x66, 0x3E, 0x36, 0x66, 0x67, 0x00}, /* U+0052 (R) */
	{ 0x1E, 0x33, 0x07, 0x0E, 0x38, 0x33, 0x1E, 0x00}, /* U+0053 (S) */
	{ 0x3F, 0x2D, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, /* U+0054 (T) */
	{ 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x3F, 0x00}, /* U+0055 (U) */
	{ 0x33, 0x33, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, /* U+0056 (V) */
	{ 0x63, 0x63, 0x63, 0x6B, 0x7F, 0x77, 0x63, 0x00}, /* U+0057 (W) */
	{ 0x63, 0x63, 0x36, 0x1C, 0x1C, 0x36, 0x63, 0x00}, /* U+0058 (X) */
	{ 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x0C, 0x1E, 0x00}, /* U+0059 (Y) */
	{ 0x7F, 0x63, 0x31, 0x18, 0x4C, 0x66, 0x7F, 0x00}, /* U+005A (Z) */
	{ 0x1E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x1E, 0x00}, /* U+005B ([) */
	{ 0x03, 0x06, 0x0C, 0x18, 0x30, 0x60, 0x40, 0x00}, /* U+005C (\) */
	{ 0x1E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x1E, 0x00}, /* U+005D (]) */
	{ 0x08, 0x1C, 0x36, 0x63, 0x00, 0x00, 0x00, 0x00}, /* U+005E (^) */
	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF}, /* U+005F (_) */
	{ 0x0C, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00}, /* U+0060 (`) */
	{ 0x00, 0x00, 0x1E, 0x30, 0x3E, 0x33, 0x6E, 0x00}, /* U+0061 (a) */
	{ 0x07, 0x06, 0x06, 0x3E, 0x66, 0x66, 0x3B, 0x00}, /* U+0062 (b) */
	{ 0x00, 0x00, 0x1E, 0x33, 0x03, 0x33, 0x1E, 0x00}, /* U+0063 (c) */
	{ 0x38, 0x30, 0x30, 0x3e, 0x33, 0x33, 0x6E, 0x00}, /* U+0064 (d) */
	{ 0x00, 0x00, 0x1E, 0x33, 0x3f, 0x03, 0x1E, 0x00}, /* U+0065 (e) */
	{ 0x1C, 0x36, 0x06, 0x0f, 0x06, 0x06, 0x0F, 0x00}, /* U+0066 (f) */
	{ 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x1F}, /* U+0067 (g) */
	{ 0x07, 0x06, 0x36, 0x6E, 0x66, 0x66, 0x67, 0x00}, /* U+0068 (h) */
	{ 0x0C, 0x00, 0x0E, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, /* U+0069 (i) */
	{ 0x30, 0x00, 0x30, 0x30, 0x30, 0x33, 0x33, 0x1E}, /* U+006A (j) */
	{ 0x07, 0x06, 0x66, 0x36, 0x1E, 0x36, 0x67, 0x00}, /* U+006B (k) */
	{ 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x1E, 0x00}, /* U+006C (l) */
	{ 0x00, 0x00, 0x33, 0x7F, 0x7F, 0x6B, 0x63, 0x00}, /* U+006D (m) */
	{ 0x00, 0x00, 0x1F, 0x33, 0x33, 0x33, 0x33, 0x00}, /* U+006E (n) */
	{ 0x00, 0x00, 0x1E, 0x33, 0x33, 0x33, 0x1E, 0x00}, /* U+006F (o) */
	{ 0x00, 0x00, 0x3B, 0x66, 0x66, 0x3E, 0x06, 0x0F}, /* U+0070 (p) */
	{ 0x00, 0x00, 0x6E, 0x33, 0x33, 0x3E, 0x30, 0x78}, /* U+0071 (q) */
	{ 0x00, 0x00, 0x3B, 0x6E, 0x66, 0x06, 0x0F, 0x00}, /* U+0072 (r) */
	{ 0x00, 0x00, 0x3E, 0x03, 0x1E, 0x30, 0x1F, 0x00}, /* U+0073 (s) */
	{ 0x08, 0x0C, 0x3E, 0x0C, 0x0C, 0x2C, 0x18, 0x00}, /* U+0074 (t) */
	{ 0x00, 0x00, 0x33, 0x33, 0x33, 0x33, 0x6E, 0x00}, /* U+0075 (u) */
	{ 0x00, 0x00, 0x33, 0x33, 0x33, 0x1E, 0x0C, 0x00}, /* U+0076 (v) */
	{ 0x00, 0x00, 0x63, 0x6B, 0x7F, 0x7F, 0x36, 0x00}, /* U+0077 (w) */
	{ 0x00, 0x00, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x00}, /* U+0078 (x) */
	{ 0x00, 0x00, 0x33, 0x33, 0x33, 0x3E, 0x30, 0x1F}, /* U+0079 (y) */
	{ 0x00, 0x00, 0x3F, 0x19, 0x0C, 0x26, 0x3F, 0x00}, /* U+007A (z) */
	{ 0x38, 0x0C, 0x0C, 0x07, 0x0C, 0x0C, 0x38, 0x00}, /* U+007B ({) */
	{ 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x18, 0x00}, /* U+007C (|) */
	{ 0x07, 0x0C, 0x0C, 0x38, 0x0C, 0x0C, 0x07, 0x00}, /* U+007D (}) */
	{ 0x6E, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, /* U+007E (~) */
};

/*
 * ===========================================================================
 * x11 backend
 */
#if defined(MG_BACKEND_X11)
#include <stdlib.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

/* macros */
#define MG_MIN(x, y) ((x < y) ? x : y)
#define MG_X_EVENT_MASK ExposureMask | StructureNotifyMask | KeyPressMask | \
	KeyReleaseMask | ButtonPressMask | ButtonReleaseMask

/* types */
struct mg_state {
	int screen;
	Display *dpy;
	Window root, win;
	Atom wmdeletewin;
	GC gc;
	Colormap colormap;

	unsigned long black, white;
	unsigned long bgcolor, color;

	int closed;
};

/* global variables */
static struct mg_state mg;

int mg_width;
int mg_height;

/* internal functions */
static int
mg_handle_x_event(struct mg_event *event, XEvent *xevnt)
{
	static int pressed = 0;
	static unsigned int lastkeycode;
	switch (xevnt->type) {
	case ClientMessage:
		/* we got a message */
		if ((Atom)xevnt->xclient.data.l[0] == mg.wmdeletewin) {
			event->type = MG_QUIT;
			return 1;
		}
		break;
	case ConfigureNotify:
		/* the window has changed somehow */
		if (xevnt->xconfigure.width != mg_width || xevnt->xconfigure.height != mg_height) {
			mg_width = xevnt->xconfigure.width;
			mg_height = xevnt->xconfigure.height;
			event->type = MG_RESIZE;
			return 1;
		}
		break;
	case Expose:
		event->type = MG_REDRAW;
		return 1;
	case KeyPress:
		if (pressed && xevnt->xkey.keycode == lastkeycode) {
			lastkeycode = xevnt->xkey.keycode;
			break;
		}
		pressed = 1;
		lastkeycode = xevnt->xkey.keycode;
		event->key = (uint32_t)XLookupKeysym(&xevnt->xkey, 0);
		event->type = MG_KEYDOWN;
		return 1;
	case KeyRelease:
		if (pressed && xevnt->xkey.keycode == lastkeycode)
			pressed = 0;
		event->key = (uint32_t)XLookupKeysym(&xevnt->xkey, 0);
		event->type = MG_KEYUP;
		return 1;
	case ButtonPress:
		if (xevnt->xbutton.x >= 0 && xevnt->xbutton.y >= 0) {
			event->button = (int)xevnt->xbutton.button;
			event->x = xevnt->xbutton.x;
			event->y = xevnt->xbutton.y;
			event->type = MG_MOUSEDOWN;
			return 1;
		}
		break;
	case ButtonRelease:
		if (xevnt->xbutton.x >= 0 && xevnt->xbutton.y >= 0) {
			event->button = (int)xevnt->xbutton.button;
			event->x = xevnt->xbutton.x;
			event->y = xevnt->xbutton.y;
			event->type = MG_MOUSEUP;
			return 1;
		}
		break;
	case MotionNotify:
		if (xevnt->xmotion.x >= 0 && xevnt->xmotion.y >= 0) {
			Window dummy1, dummy2;
			int dummy3, dummy4;
			unsigned int dummy5;

			int x, y;

			if (XQueryPointer(mg.dpy, mg.win, &dummy1, &dummy2, &dummy3,
						&dummy4, &x, &y, &dummy5)) {
				if (x >= 0 && y >= 0 && (event->x != x || event->y != y)) {
					event->x = x;
					event->y = y;
					event->type = MG_MOUSEMOTION;
					return 1;
				}
			}
		}
		break;
	}
	return 0;
}

/* drawing functions */
void
mg_clear(void)
{
	XClearWindow(mg.dpy, mg.win);
}

void 
mg_drawcircle(int x, int y, int r)
{
	XDrawArc(mg.dpy, mg.win, mg.gc, x - r, y - r, (unsigned int)r * 2, (unsigned int)r * 2, 0, 360 * 64);
}

void
mg_drawline(int x1, int y1, int x2, int y2)
{
	XDrawLine(mg.dpy, mg.win, mg.gc, x1, y1, x2, y2);
}

void
mg_drawpixel(int x, int y)
{
	XDrawPoint(mg.dpy, mg.win, mg.gc, x, y);
}

void
mg_drawrect(int x1, int y1, int x2, int y2)
{
	XDrawRectangle(mg.dpy, mg.win, mg.gc, MG_MIN(x1, x2), MG_MIN(y1, y2),
			(unsigned int)abs(x2 - x1), (unsigned int)abs(y2 - y1));
}

void
mg_drawtext(int x, int y, const char *text, int size)
{
	if (size < 1) {
		return;
	} else if (size == 1) {
		XPoint letter[64];
		size_t i;
		int points = 0;
		int px, py; /* point x, point y */
		int dx = x; /* draw x */

		for (i = 0; i < strlen(text); ++i) {
			for (py = 0; py < 8; ++py) {
				for (px = 0; px < 8; ++px) {
					if (text[i] >= 0x20 && text[i] <= 0x7e &&
							(mg_font[text[i] - 0x20][py] & 1 << px)) {
						letter[points].x = (short)(dx + px);
						letter[points].y = (short)(y + py);
						++points;
					}
				}
			}
			dx += 8;
			if (points)
				XDrawPoints(mg.dpy, mg.win, mg.gc, letter, points, CoordModeOrigin);
			points = 0;
		}
	} else {
		XRectangle letter[64];
		size_t i;
		int points = 0;
		int px, py; /* point x, point y */
		int dx = x; /* draw x */

		for (i = 0; i < strlen(text); ++i) {
			for (py = 0; py < 8; ++py) {
				for (px = 0; px < 8; ++px) {
					if (text[i] >= 0x20 && text[i] <= 0x7e &&
							(mg_font[text[i] - 0x20][py] & 1 << px)) {
						letter[points].x = (short)(dx + (px * size));
						letter[points].y = (short)(y + (py * size));
						letter[points].width = (unsigned short)size;
						letter[points].height = (unsigned short)size;
						++points;
					}
				}
			}
			dx += 8 * size;
			if (points)
				XFillRectangles(mg.dpy, mg.win, mg.gc, letter, points);
			points = 0;
		}
	}
}

void
mg_drawtriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	XDrawLine(mg.dpy, mg.win, mg.gc, x1, y1, x2, y2);
	XDrawLine(mg.dpy, mg.win, mg.gc, x2, y2, x3, y3);
	XDrawLine(mg.dpy, mg.win, mg.gc, x3, y3, x1, y1);
}

void
mg_fillcircle(int x, int y, int r)
{
	XFillArc(mg.dpy, mg.win, mg.gc, x - r, y - r, (unsigned int)r * 2, (unsigned int)r * 2, 0, 360 * 64);
}

void
mg_fillrect(int x1, int y1, int x2, int y2)
{
	XFillRectangle(mg.dpy, mg.win, mg.gc, MG_MIN(x1, x2), MG_MIN(y1, y2),
			(unsigned int)abs(x2 - x1), (unsigned int)abs(y2 - y1));
}

void
mg_filltriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	XPoint points[3];
	points[0].x = (short)x1;
	points[0].y = (short)y1;
	points[1].x = (short)x2;
	points[1].y = (short)y2;
	points[2].x = (short)x3;
	points[2].y = (short)y3;
	XFillPolygon(mg.dpy, mg.win, mg.gc, points, 3, Convex, CoordModeOrigin);
}

void
mg_flush(void)
{
	XFlush(mg.dpy);
}

/* functions to change options */
void
mg_setbgcolor(uint8_t r, uint8_t g, uint8_t b)
{
	static int unfreed = 0;

	if (unfreed) {
		XFreeColors(mg.dpy, mg.colormap, &mg.bgcolor, 1, 0);
		unfreed = 0;
	}

	if (r == 0 && g == 0 && b == 0) {
		mg.bgcolor = mg.black;
	} else if (r == 25 && g == 255 && b == 255) {
		mg.bgcolor = mg.white;
	} else {
		XColor color;
		color.red = r * 257;
		color.green = g * 257;
		color.blue = b * 257;

		unfreed = 1;
		XAllocColor(mg.dpy, mg.colormap, &color);
		mg.bgcolor = color.pixel;
	}
	XSetBackground(mg.dpy, mg.gc, mg.bgcolor);
	XSetWindowBackground(mg.dpy, mg.win, mg.bgcolor);
}

void
mg_setdrawcolor(uint8_t r, uint8_t g, uint8_t b)
{
	static int unfreed = 0;

	if (unfreed) {
		XFreeColors(mg.dpy, mg.colormap, &mg.color, 1, 0);
		unfreed = 0;
	}

	if (r == 0 && g == 0 && b == 0) {
		mg.color = mg.black;
	} else if (r == 25 && g == 255 && b == 255) {
		mg.color = mg.white;
	} else {
		XColor color;
		color.red = r * 257;
		color.green = g * 257;
		color.blue = b * 257;

		unfreed = 1;
		XAllocColor(mg.dpy, mg.colormap, &color);
		mg.color = color.pixel;
	}
	XSetForeground(mg.dpy, mg.gc, mg.color);
}

/* events */
void
mg_waitevent(struct mg_event *event)
{
	XEvent xevnt;
	for (;;) {
		/* wait for an X event to happen */
		XNextEvent(mg.dpy, &xevnt);

		/* handle event */
		if (mg_handle_x_event(event, &xevnt))
			break;
	}
}

/* initialization & shutdown */
int
mg_init(int w, int h, const char *title)
{
	XEvent xevnt;

	/* open display */
	if (!(mg.dpy = XOpenDisplay(NULL)))
		return -1;

	/* set detectable autorepeat */
	XkbSetDetectableAutoRepeat(mg.dpy, True, NULL);

	/* set screen, root window, colormap, and background color */
	mg.screen = DefaultScreen(mg.dpy);
	mg.root = DefaultRootWindow(mg.dpy);
	mg.colormap = DefaultColormap(mg.dpy, mg.screen);
	mg.bgcolor = mg.black = BlackPixel(mg.dpy, mg.screen);
	mg.color = mg.white = WhitePixel(mg.dpy, mg.screen);

	/* create window */
	mg.win = XCreateSimpleWindow(mg.dpy, mg.root, 0, 0,
			(unsigned int)w, (unsigned int)h, 0, mg.bgcolor, mg.bgcolor);
	mg.closed = 0;
	mg_width = w;
	mg_height = h;

	/* name our window */
	XStoreName(mg.dpy, mg.win, title);

	/* specify WM_DELETE_WINDOW protocol */
	mg.wmdeletewin = XInternAtom(mg.dpy, "WM_DELETE_WINDOW", 1);
	XSetWMProtocols(mg.dpy, mg.win, &mg.wmdeletewin, 1);

	/* tell X what events we are interested in */
	XSelectInput(mg.dpy, mg.win, MG_X_EVENT_MASK | PointerMotionMask);

	/* map window */
	XMapWindow(mg.dpy, mg.win);

	/* create graphics context */
	mg.gc = XCreateGC(mg.dpy, mg.win, 0, NULL);

	/* set GC colors */
	XSetBackground(mg.dpy, mg.gc, mg.bgcolor);
	XSetWindowBackground(mg.dpy, mg.win, mg.bgcolor);
	XSetForeground(mg.dpy, mg.gc, mg.color);

	/* wait for window mapping */
	for (;;) {
		XNextEvent(mg.dpy, &xevnt);
		if (xevnt.type == ConfigureNotify) {
			mg_width = xevnt.xconfigure.width;
			mg_height = xevnt.xconfigure.height;
		} else if (xevnt.type == MapNotify) {
			break;
		}
	}
	return 0;
}

void
mg_quit(void)
{
	if (!mg.closed) {
		XFreeGC(mg.dpy, mg.gc);
		XDestroyWindow(mg.dpy, mg.win);
		XCloseDisplay(mg.dpy);
		mg.closed = 1;
	}
}
#endif /* defined(MG_BACKEND_X11) */

/*
 * ===========================================================================
 * wayland backend
 */
#if defined(MG_BACKEND_WAYLAND)
#if defined(__linux__)
#include <linux/input-event-codes.h>
#else
#define BTN_MOUSE  0x110
#define BTN_LEFT   0x110
#define BTN_RIGHT  0x111
#define BTN_MIDDLE 0x112
#define BTN_SIDE   0x113
#define BTN_EXTRA  0x114
#endif /* defined(__linux__) */

#include <sys/mman.h>

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <wayland-client.h>
#include <xkbcommon/xkbcommon.h>

#include "xdg-shell-client-protocol.h"

/* macros */
#define MG_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MG_MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MG_PIXELSET(x, y) \
	if ((x) >= 0 && (y) >= 0 && (x) < (int)mg.buf_width && (y) < (int)mg.buf_height) \
		mg.draw_buf[(y) * (int)mg.buf_width + (x)] = mg.color
#define MG_PIXELSET_NOBOUNDSCHECK(x, y) \
	mg.draw_buf[(y) * (int)mg.buf_width + (x)] = mg.color

/* types */
enum mg_pointer_event_mask {
	MG_POINTER_EVENT_ENTER = 1 << 0,
	MG_POINTER_EVENT_LEAVE = 1 << 1,
	MG_POINTER_EVENT_MOTION = 1 << 2,
	MG_POINTER_EVENT_BUTTON = 1 << 3,
	MG_POINTER_EVENT_AXIS = 1 << 4,
	MG_POINTER_EVENT_AXIS_SOURCE = 1 << 5,
	MG_POINTER_EVENT_AXIS_STOP = 1 << 6,
	MG_POINTER_EVENT_AXIS_DISCRETE = 1 << 7,
};

struct mg_pointer_event {
	uint32_t event_mask;
	wl_fixed_t surface_x, surface_y;
	uint32_t button, state;
	uint32_t time;
	uint32_t serial;
	struct {
		int valid;
		wl_fixed_t value;
		int32_t discrete;
	} axes[2];
	uint32_t axis_source;
};

struct mg_state {
	/* globals */
	struct wl_display *wl_display;
	struct wl_registry *wl_registry;
	struct wl_shm *wl_shm;
	struct wl_compositor *wl_compositor;
	struct xdg_wm_base *xdg_wm_base;
	struct wl_seat *wl_seat;
	/* objects */
	struct wl_surface *wl_surface;
	struct xdg_surface *xdg_surface;
	struct wl_keyboard *wl_keyboard;
	struct wl_pointer *wl_pointer;
	struct wl_touch *wl_touch;
	struct xdg_toplevel *xdg_toplevel;
	/* state */
	uint32_t last_frame;
	struct mg_pointer_event mg_pointer_event;
	struct xkb_state *xkb_state;
	struct xkb_context *xkb_context;
	struct xkb_keymap *xkb_keymap;

	int closed;
	uint32_t *draw_buf;
	uint32_t bgcolor, color;
	size_t buf_width, buf_height, buf_stride, buf_size;

	/* highest to lowest priority */
	int pending_quit;
	int pending_resize;
	uint32_t pending_key_down;
	uint32_t pending_key_up;
	struct mg_event pending_mouse_down;
	struct mg_event pending_mouse_up;
	struct mg_event pending_mouse_motion;
};

/* global variables */
static struct mg_state mg;

int mg_width;
int mg_height;

/* internal utility functions */
static int
mg_clamp(int val, int min, int max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;
	else
		return val;
}

static void
mg_fillflatsidetriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	/* points 2 and 3 must be on the same horizontal line (y2 == y3) */
	int tmp1_x = x1, tmp1_y = y1;
	int tmp2_x = x1, tmp2_y = y1;

	int changed1 = 0, changed2 = 0;

	int dx1 = abs(x2 - x1);
	int dy1 = abs(y2 - y1);

	int dx2 = abs(x3 - x1);
	int dy2 = abs(y3 - y1);

	int sx1 = (x2 - x1 > 0) ? 1 : -1;
	int sx2 = (x3 - x1 > 0) ? 1 : -1;

	int sy1 = (y2 - y1 > 0) ? 1 : -1;
	int sy2 = (y3 - y1 > 0) ? 1 : -1;

	int e1, e2;

	int i, tmp;
	int lowx, highx;

	if (dy1 > dx1) {
		tmp = dx1;
		dx1 = dy1;
		dy1 = tmp;
		changed1 = 1;
	}

	if (dy2 > dx2) {
		tmp = dx2;
		dx2 = dy2;
		dy2 = tmp;
		changed2 = 1;
	}

	e1 = 2 * dy1 - dx1;
	e2 = 2 * dy2 - dx2;

	for (i = 0; i <= dx1; ++i) {
		/* mg_drawline(tmp1_x, tmp1_y, tmp2_x, tmp2_y); */

		if (tmp1_y >= 0 && tmp1_y < (int)mg.buf_height) {
			/* assuming tmp1_y == tmp2_y */
			lowx = mg_clamp(MG_MIN(tmp1_x, tmp2_x), 0, (int)mg.buf_width - 1);
			highx = mg_clamp(MG_MAX(tmp1_x, tmp2_x), 0, (int)mg.buf_width - 1);
			for (; lowx <= highx; ++lowx)
				MG_PIXELSET_NOBOUNDSCHECK(lowx, tmp1_y);
		}

		while (e1 >= 0) {
			if (changed1)
				tmp1_x += sx1;
			else
				tmp1_y += sy1;
			e1 = e1 - 2 * dx1;
		}

		if (changed1)
			tmp1_y += sy1;
		else
			tmp1_x += sx1;

		e1 = e1 + 2 * dy1;

		while (tmp2_y != tmp1_y) {
			while (e2 >= 0) {
				if (changed2)
					tmp2_x += sx2;
				else
					tmp2_y += sy2;
				e2 = e2 - 2 * dx2;
			}

			if (changed2)
				tmp2_y += sy2;
			else
				tmp2_x += sx2;

			e2 = e2 + 2 * dy2;
		}
	}
}

static void
mg_frametrimcpy(uint32_t *dst, const uint32_t *src, size_t oldwidth, size_t newwidth,
		size_t oldstride, size_t newstride, size_t oldheight, size_t newheight)
{
	size_t roff = 0, woff = 0;
	size_t stride = MG_MIN(oldstride, newstride);
	size_t i = 0;
	for (; i < MG_MIN(oldheight, newheight) &&
			(roff < oldwidth * oldheight) &&
			(woff < newwidth * newheight); ++i) {
		memcpy(dst + woff, src + roff, stride);
		roff += oldwidth;
		woff += newwidth;
	}
}

static int
mg_handle_wl_event(struct mg_event *event)
{
	if (mg.pending_quit) {
		event->type = MG_QUIT;
		mg.pending_quit = 0;
		return 1;
	} else if (mg.pending_resize) {
		event->type = MG_RESIZE;
		mg.pending_resize = 0;
		return 1;
	} else if (mg.pending_key_down) {
		event->type = MG_KEYDOWN;
		event->key = mg.pending_key_down;
		mg.pending_key_down = 0;
		return 1;
	} else if (mg.pending_key_up) {
		event->type = MG_KEYUP;
		event->key = mg.pending_key_up;
		mg.pending_key_up = 0;
		return 1;
	} else if (mg.pending_mouse_down.type != MG_NOEVENT) {
		event->type = MG_MOUSEDOWN;
		event->button = mg.pending_mouse_down.button;
		event->x = mg.pending_mouse_down.x;
		event->y = mg.pending_mouse_down.y;
		mg.pending_mouse_down.type = MG_NOEVENT;
		return 1;
	} else if (mg.pending_mouse_up.type != MG_NOEVENT) {
		event->type = MG_MOUSEUP;
		event->button = mg.pending_mouse_up.button;
		event->x = mg.pending_mouse_up.x;
		event->y = mg.pending_mouse_up.y;
		mg.pending_mouse_up.type = MG_NOEVENT;
		return 1;
	} else if (mg.pending_mouse_motion.type != MG_NOEVENT) {
		event->type = MG_MOUSEMOTION;
		event->x = mg.pending_mouse_motion.x;
		event->y = mg.pending_mouse_motion.y;
		mg.pending_mouse_motion.type = MG_NOEVENT;
		return 1;
	}
	return 0;
}

static void
mg_sort_ascending_by_y(int x1, int y1, int x2, int y2, int x3, int y3,
		int *nx1, int *ny1, int *nx2, int *ny2, int *nx3, int *ny3)
{
#define SWAP(x, y) tmp = x; x = y; y = tmp;
	int tmp;
	if (y1 > y3) {
		SWAP(y1, y3);
		SWAP(x1, x3);
	}
	if (y1 > y2) {
		SWAP(y1, y2);
		SWAP(x1, x2);
	}
	if (y2 > y3) {
		SWAP(y2, y3);
		SWAP(x2, x3);
	}
#undef SWAP

	/* remember that lower Y value = higher */
	*nx1 = x3;
	*ny1 = y3;
	*nx2 = x2;
	*ny2 = y2;
	*nx3 = x1;
	*ny3 = y1;
}

/* shared memory stuff */
static void
mg_randname(char *buf)
{
	/* generate a (pretty bad) random filename. */
	struct timespec ts;
	long r;
	clock_gettime(CLOCK_REALTIME, &ts);
	r = ts.tv_nsec;
	for (int i = 0; i < 6; ++i) {
		buf[i] = (char)('A'+(r&15)+(r&16)*2);
		r >>= 5;
	}
}

static int
mg_create_shm_file(void)
{
	int retries = 100;
	int fd;
	do {
		char name[] = "/mg-wl_shm-XXXXXX";
		mg_randname(name + sizeof(name) - 7);
		retries--;
		fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, 0600);
		if (fd >= 0) {
			shm_unlink(name);
			return fd;
		}
	} while (retries > 0 && errno == EEXIST);
	return -1;
}

static int
mg_allocate_shm_file(size_t size)
{
	int fd = mg_create_shm_file();
	int ret;
	if (fd < 0)
		return -1;
	do {
		/*
		 * despite what it sounds like, ftruncate can
		 * actually increase the size of the file,
		 * that's what we're doing here
		 */
		ret = ftruncate(fd, (off_t)size);
	} while (ret < 0 && errno == EINTR);
	if (ret < 0) {
		close(fd);
		return -1;
	}
	return fd;
}

/* wayland event handling */
static void
mg_wl_pointer_enter(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t serial, __attribute__((__unused__)) struct wl_surface *surface,
		wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	/* pointer entered our surface */
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_ENTER;
	mg_state->mg_pointer_event.serial = serial;
	mg_state->mg_pointer_event.surface_x = surface_x;
	mg_state->mg_pointer_event.surface_y = surface_y;
}

static void
mg_wl_pointer_leave(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t serial, __attribute__((__unused__)) struct wl_surface *surface)
{
	/* pointer left our surface */
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.serial = serial;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_LEAVE;
}

static void
mg_wl_pointer_motion(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t time, wl_fixed_t surface_x, wl_fixed_t surface_y)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_MOTION;
	mg_state->mg_pointer_event.time = time;
	mg_state->mg_pointer_event.surface_x = surface_x;
	mg_state->mg_pointer_event.surface_y = surface_y;
}

static void
mg_wl_pointer_button(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_BUTTON;
	mg_state->mg_pointer_event.time = time;
	mg_state->mg_pointer_event.serial = serial;
	mg_state->mg_pointer_event.button = button;
	mg_state->mg_pointer_event.state = state;
}

static void
mg_wl_pointer_axis(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t time, uint32_t axis, wl_fixed_t value)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_AXIS;
	mg_state->mg_pointer_event.time = time;
	mg_state->mg_pointer_event.axes[axis].valid = 1;
	mg_state->mg_pointer_event.axes[axis].value = value;
}

static void
mg_wl_pointer_axis_source(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t axis_source)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_AXIS_SOURCE;
	mg_state->mg_pointer_event.axis_source = axis_source;
}

static void
mg_wl_pointer_axis_stop(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t time, uint32_t axis)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.time = time;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_AXIS_STOP;
	mg_state->mg_pointer_event.axes[axis].valid = 1;
}

static void
mg_wl_pointer_axis_discrete(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer,
		uint32_t axis, int32_t discrete)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->mg_pointer_event.event_mask |= MG_POINTER_EVENT_AXIS_DISCRETE;
	mg_state->mg_pointer_event.axes[axis].valid = 1;
	mg_state->mg_pointer_event.axes[axis].discrete = discrete;
}

static void
mg_wl_pointer_frame(void *data, __attribute__((__unused__)) struct wl_pointer *wl_pointer)
{
	/* some pointer event happened, this is where we handle these */
	struct mg_state *mg_state = (struct mg_state *)data;
	struct mg_pointer_event *event = &mg_state->mg_pointer_event;

	if ((event->event_mask & MG_POINTER_EVENT_ENTER) ||
			(event->event_mask & MG_POINTER_EVENT_MOTION)) {
		mg_state->pending_mouse_motion.type = MG_MOUSEMOTION;
		mg_state->pending_mouse_motion.x = wl_fixed_to_int(event->surface_x);
		mg_state->pending_mouse_motion.y = wl_fixed_to_int(event->surface_y);
	}

	if (event->event_mask & MG_POINTER_EVENT_BUTTON && event->button >= BTN_LEFT &&
			event->button <= BTN_EXTRA) {
		int button;
		switch (event->button) {
		case BTN_LEFT:
			button = 1;
			break;
		case BTN_RIGHT:
			button = 3;
			break;
		case BTN_MIDDLE:
			button = 2;
			break;
		case BTN_SIDE:
			button = 4;
			break;
		case BTN_EXTRA:
			button = 5;
			break;
		}
		if (event->state == WL_POINTER_BUTTON_STATE_PRESSED) {
			mg_state->pending_mouse_down.type = MG_MOUSEDOWN;
			mg_state->pending_mouse_down.button = button;
			mg_state->pending_mouse_down.x = wl_fixed_to_int(event->surface_x);
			mg_state->pending_mouse_down.y = wl_fixed_to_int(event->surface_y);
		} else {
			mg_state->pending_mouse_up.type = MG_MOUSEUP;
			mg_state->pending_mouse_up.button = button;
			mg_state->pending_mouse_up.x = wl_fixed_to_int(event->surface_x);
			mg_state->pending_mouse_up.y = wl_fixed_to_int(event->surface_y);
		}
	}

	memset(event, 0, sizeof(*event));
}

static const struct wl_pointer_listener mg_wl_pointer_listener = {
	.enter = mg_wl_pointer_enter,
	.leave = mg_wl_pointer_leave,
	.motion = mg_wl_pointer_motion,
	.button = mg_wl_pointer_button,
	.axis = mg_wl_pointer_axis,
	.frame = mg_wl_pointer_frame,
	.axis_source = mg_wl_pointer_axis_source,
	.axis_stop = mg_wl_pointer_axis_stop,
	.axis_discrete = mg_wl_pointer_axis_discrete,
};

/* keyboard */
static void
mg_wl_keyboard_keymap(void *data, __attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		uint32_t format, int32_t fd, uint32_t size)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	char *map_shm;
	struct xkb_keymap *xkb_keymap;
	struct xkb_state *xkb_state;

	if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
		/* TODO: fix this */
		fputs("mg: unsupported keymap format, this will be fixed later\n", stderr);
		exit(1);
	}

	map_shm = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (map_shm == MAP_FAILED) {
		/* TODO: fix this */
		fprintf(stderr, "mg: mmap failed: %s\n", strerror(errno));
		exit(1);
	}

	/* configure the keymap */
	xkb_keymap = xkb_keymap_new_from_string(mg_state->xkb_context, map_shm,
			XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
	munmap(map_shm, size);
	close(fd);

	xkb_state = xkb_state_new(xkb_keymap);
	xkb_keymap_unref(mg_state->xkb_keymap);
	xkb_state_unref(mg_state->xkb_state);
	mg_state->xkb_keymap = xkb_keymap;
	mg_state->xkb_state = xkb_state;
}

static void
mg_wl_keyboard_enter(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial,
		__attribute__((__unused__)) struct wl_surface *surface,
		__attribute__((__unused__)) struct wl_array *keys)
{
	/* TODO: fire MG_KEYDOWN event for each key */
	/*
	struct mg_state *mg_state = (struct mg_state *)data;
	fputs("keyboard enter; keys pressed are:\n", stderr);
	uint32_t *key;
	wl_array_for_each(key, keys) {
		char buf[128];
		xkb_keysym_t sym = xkb_state_key_get_one_sym(
				mg_state->xkb_state, *key + 8);
		xkb_keysym_get_name(sym, buf, sizeof(buf));
		fprintf(stderr, "sym: %-12s (%d), ", buf, sym);
		xkb_state_key_get_utf8(mg_state->xkb_state,
				*key + 8, buf, sizeof(buf));
		fprintf(stderr, "utf8: '%s'\n", buf);
	}
	*/
}

static void
mg_wl_keyboard_key(void *data, __attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial,
		__attribute__((__unused__)) uint32_t time, uint32_t key, uint32_t state)
{
	/* key press/release event */
	struct mg_state *mg_state = (struct mg_state *)data;
	if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
		mg_state->pending_key_down = xkb_state_key_get_one_sym(mg_state->xkb_state, key + 8);
	else
		mg_state->pending_key_up = xkb_state_key_get_one_sym(mg_state->xkb_state, key + 8);
}

static void
mg_wl_keyboard_leave(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial,
		__attribute__((__unused__)) struct wl_surface *surface)
{
	/*fputs("keyboard leave\n", stderr);*/
}

static void
mg_wl_keyboard_modifiers(void *data, __attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) uint32_t serial, uint32_t mods_depressed,
		uint32_t mods_latched, uint32_t mods_locked,
		uint32_t group)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	xkb_state_update_mask(mg_state->xkb_state,
			mods_depressed, mods_latched, mods_locked, 0, 0, group);
}

static void
mg_wl_keyboard_repeat_info(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_keyboard *wl_keyboard,
		__attribute__((__unused__)) int32_t rate,
		__attribute__((__unused__)) int32_t delay)
{
	/* stub */
}

static const struct wl_keyboard_listener mg_wl_keyboard_listener = {
	.keymap = mg_wl_keyboard_keymap,
	.enter = mg_wl_keyboard_enter,
	.leave = mg_wl_keyboard_leave,
	.key = mg_wl_keyboard_key,
	.modifiers = mg_wl_keyboard_modifiers,
	.repeat_info = mg_wl_keyboard_repeat_info,
};

static void
mg_wl_buffer_release(__attribute__((__unused__)) void *data, struct wl_buffer *wl_buffer)
{
	/* sent by the compositor when it's no longer using this buffer */
	wl_buffer_destroy(wl_buffer);
}

static const struct wl_buffer_listener mg_wl_buffer_listener = {
	.release = mg_wl_buffer_release,
};

static struct wl_buffer *
mg_draw_frame(struct mg_state *mg_state)
{
	size_t stride = (size_t)mg_width * 4;
	size_t size = stride * (size_t)mg_height;

	int fd = mg_allocate_shm_file(size);
	uint32_t *data;
	struct wl_shm_pool *pool;
	struct wl_buffer *buffer;

	if (fd == -1)
		return NULL;

	/*
	 * notice the MAP_SHARED flag. this is because we (obviously)
	 * want others to be able to read the contents of data
	 */
	data = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (data == MAP_FAILED) {
		fputs("mg: mmap failed, not drawing this frame\n", stderr);
		close(fd);
		return NULL;
	}

	/* create a wayland buffer object */
	pool = wl_shm_create_pool(mg_state->wl_shm, fd, (int32_t)size);
	buffer = wl_shm_pool_create_buffer(pool, 0, (int32_t)mg_width, (int32_t)mg_height,
			(int32_t)stride, WL_SHM_FORMAT_XRGB8888);
	wl_shm_pool_destroy(pool);
	close(fd);

	memcpy(data, mg_state->draw_buf, mg_state->buf_size);

	munmap(data, size);
	wl_buffer_add_listener(buffer, &mg_wl_buffer_listener, NULL);
	return buffer;
}

static void
mg_xdg_toplevel_configure(void *data,
		__attribute__((__unused__)) struct xdg_toplevel *xdg_toplevel,
		int32_t width, int32_t height, __attribute__((__unused__)) struct wl_array *states)
{
	struct mg_state *mg_state = (struct mg_state *)data;

	if (width == 0 || height == 0)
		/* compositor is deferring to us */
		return;

	if (width != (int32_t)mg_width || height != (int32_t)mg_height) {
		/* draw_buf needs resizing */
		size_t stride, size;
		uint32_t *new_draw_buf;

		mg_width = width;
		mg_height = height;
		stride = (size_t)mg_width * 4;
		size = stride * (size_t)mg_height;

		if (!(new_draw_buf = calloc(size, 1))) {
			fputs("mg: malloc: out of memory\n", stderr);
			exit(1);
		}
		mg_frametrimcpy(new_draw_buf, mg_state->draw_buf,
			mg_state->buf_width, (size_t)mg_width,
			mg_state->buf_stride, stride,
			mg_state->buf_height, (size_t)mg_height);
		free(mg_state->draw_buf);
		mg_state->draw_buf = new_draw_buf;
		mg_state->buf_width = (size_t)mg_width;
		mg_state->buf_stride = stride;
		mg_state->buf_height = (size_t)mg_height;
		mg_state->buf_size = stride * (size_t)mg_height;

		mg_state->pending_resize = 1;
	}
}

static void
mg_xdg_toplevel_close(void *data, __attribute__((__unused__)) struct xdg_toplevel *toplevel)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	mg_state->pending_quit = 1;
}

static const struct xdg_toplevel_listener mg_xdg_toplevel_listener = {
	.configure = mg_xdg_toplevel_configure,
	.close = mg_xdg_toplevel_close,
};

static void
mg_xdg_surface_configure(void *data,
		struct xdg_surface *xdg_surface, uint32_t serial)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	struct wl_buffer *buffer;
	xdg_surface_ack_configure(xdg_surface, serial);

	buffer = mg_draw_frame(mg_state);
	wl_surface_attach(mg_state->wl_surface, buffer, 0, 0);
	wl_surface_commit(mg_state->wl_surface);
}

static const struct xdg_surface_listener mg_xdg_surface_listener = {
	.configure = mg_xdg_surface_configure,
};

static void
mg_xdg_wm_base_ping(__attribute__((__unused__)) void *data,
		struct xdg_wm_base *xdg_wm_base, uint32_t serial)
{
	xdg_wm_base_pong(xdg_wm_base, serial);
}

static const struct xdg_wm_base_listener mg_xdg_wm_base_listener = {
	.ping = mg_xdg_wm_base_ping,
};

static void
mg_wl_seat_capabilities(void *data, __attribute__((__unused__)) struct wl_seat *wl_seat,
		uint32_t capabilities)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	int have_keyboard;

	int have_pointer = capabilities & WL_SEAT_CAPABILITY_POINTER;

	if (have_pointer && mg_state->wl_pointer == NULL) {
		mg_state->wl_pointer = wl_seat_get_pointer(mg_state->wl_seat);
		wl_pointer_add_listener(mg_state->wl_pointer,
				&mg_wl_pointer_listener, mg_state);
	} else if (!have_pointer && mg_state->wl_pointer != NULL) {
		wl_pointer_release(mg_state->wl_pointer);
		mg_state->wl_pointer = NULL;
	}

	have_keyboard = capabilities & WL_SEAT_CAPABILITY_KEYBOARD;

	if (have_keyboard && mg_state->wl_keyboard == NULL) {
		mg_state->wl_keyboard = wl_seat_get_keyboard(mg_state->wl_seat);
		wl_keyboard_add_listener(mg_state->wl_keyboard,
				&mg_wl_keyboard_listener, mg_state);
	} else if (!have_keyboard && mg_state->wl_keyboard != NULL) {
		wl_keyboard_release(mg_state->wl_keyboard);
		mg_state->wl_keyboard = NULL;
	}
}

static void
mg_wl_seat_name(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_seat *wl_seat,
		__attribute__((__unused__)) const char *name)
{
	/* stub */
}

static const struct wl_seat_listener mg_wl_seat_listener = {
	.capabilities = mg_wl_seat_capabilities,
	.name = mg_wl_seat_name,
};

static void
mg_registry_global(void *data, struct wl_registry *wl_registry,
		uint32_t name, const char *interface,
		__attribute__((__unused__)) uint32_t version)
{
	struct mg_state *mg_state = (struct mg_state *)data;
	if (strcmp(interface, wl_shm_interface.name) == 0) {
		mg_state->wl_shm = wl_registry_bind(
				wl_registry, name, &wl_shm_interface, 1);
	} else if (strcmp(interface, wl_compositor_interface.name) == 0) {
		mg_state->wl_compositor = wl_registry_bind(
				wl_registry, name, &wl_compositor_interface, 4);
	} else if (strcmp(interface, xdg_wm_base_interface.name) == 0) {
		mg_state->xdg_wm_base = wl_registry_bind(
				wl_registry, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(mg_state->xdg_wm_base,
				&mg_xdg_wm_base_listener, mg_state);
	} else if (strcmp(interface, wl_seat_interface.name) == 0) {
		mg_state->wl_seat = wl_registry_bind(
				wl_registry, name, &wl_seat_interface, 7);
		wl_seat_add_listener(mg_state->wl_seat,
				&mg_wl_seat_listener, mg_state);
	}
}

static void
mg_registry_global_remove(__attribute__((__unused__)) void *data,
		__attribute__((__unused__)) struct wl_registry *wl_registry,
		__attribute__((__unused__)) uint32_t name)
{
	/* stub */
}

static const struct wl_registry_listener wl_registry_listener = {
	.global = mg_registry_global,
	.global_remove = mg_registry_global_remove,
};

/* drawing functions */
void
mg_clear(void)
{
	size_t i = 0;
	for (; i < mg.buf_width * mg.buf_height; ++i)
		mg.draw_buf[i] = mg.bgcolor;
}

void
mg_drawcircle(int x, int y, int r)
{
	int dx = -r, dy = 0, err = 2 - 2 * r;
	do {
		MG_PIXELSET(x - dx, y + dy);
		MG_PIXELSET(x - dy, y - dx);
		MG_PIXELSET(x + dx, y - dy);
		MG_PIXELSET(x + dy, y + dx);

		r = err;
		if (r <= dy)
			err += ++dy * 2 + 1;
		if (r > dx || err > dy)
			err += ++dx * 2 + 1;
	} while (dx < 0);
}

void
mg_drawline(int x1, int y1, int x2, int y2)
{
	x1 = mg_clamp(x1, 0, (int)mg.buf_width - 1);
	y1 = mg_clamp(y1, 0, (int)mg.buf_height - 1);
	x2 = mg_clamp(x2, 0, (int)mg.buf_width - 1);
	y2 = mg_clamp(y2, 0, (int)mg.buf_height - 1);
	if (x1 == x2) {
		int lowy = MG_MIN(y1, y2);
		int highy = MG_MAX(y1, y2);
		for (; lowy <= highy; ++lowy)
			MG_PIXELSET_NOBOUNDSCHECK(x1, lowy);
	} else if (y1 == y2) {
		int lowx = MG_MIN(x1, x2);
		int highx = MG_MAX(x1, x2);
		for (; lowx <= highx; ++lowx)
			MG_PIXELSET_NOBOUNDSCHECK(lowx, y1);
	} else {
		int dx = abs(x2 - x1), sx = (x1 < x2) ? 1 : -1;
		int dy = -abs(y2 - y1), sy = (y1 < y2) ? 1 : -1;
		int err = dx + dy, e2;

		for (;;) {
			MG_PIXELSET_NOBOUNDSCHECK(x1, y1);
			if (x1 == x2 && y1 == y2)
				break;
			e2 = err * 2;
			if (e2 >= dy) {
				err += dy;
				x1 += sx;
			}
			if (e2 <= dx) {
				err += dx;
				y1 += sy;
			}
		}
	}
}

void
mg_drawpixel(int x, int y)
{
	MG_PIXELSET(x, y);
}

void
mg_drawrect(int x1, int y1, int x2, int y2)
{
	mg_drawline(x1, y1, x2, y1); /* top */
	mg_drawline(x2, y1, x2, y2); /* right */
	mg_drawline(x1, y2, x2, y2); /* bottom */
	mg_drawline(x1, y1, x1, y2); /* left */
}

void
mg_drawtext(int x, int y, const char *text, int size)
{
	if (size < 1) {
		return;
	} else {
		size_t i;
		int px, py; /* point x, point y */
		int dx = x; /* draw x */

		for (i = 0; i < strlen(text); ++i) {
			for (py = 0; py < 8; ++py) {
				for (px = 0; px < 8; ++px) {
					if (text[i] >= 0x20 && text[i] <= 0x7e &&
							(mg_font[text[i] - 0x20][py] & 1 << px)) {
						int tmpx = dx + (px * size);
						int tmpy = y + (py * size);
						if (size > 1)
							mg_fillrect(tmpx, tmpy, tmpx + size - 1, tmpy + size - 1);
						else
							MG_PIXELSET(tmpx, tmpy);
					}
				}
			}
			dx += 8 * size;
		}
	}
}

void
mg_drawtriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	mg_drawline(x1, y1, x2, y2);
	mg_drawline(x2, y2, x3, y3);
	mg_drawline(x3, y3, x1, y1);
}

void
mg_fillcircle(int x, int y, int r)
{
	/* TODO: this is really bad.... */
	int dx, dy = -r;
	for (; dy <= r; ++dy)
		for (dx = -r; dx <= r; ++dx)
			if (dx * dx + dy * dy < r * r + r)
				MG_PIXELSET(dx + x, dy + y);
}

void
mg_fillrect(int x1, int y1, int x2, int y2)
{
	int x;
	int lowx = mg_clamp(MG_MIN(x1, x2), 0, (int)mg.buf_width - 1);
	int highx = mg_clamp(MG_MAX(x1, x2), 0, (int)mg.buf_width - 1);
	int lowy = mg_clamp(MG_MIN(y1, y2), 0, (int)mg.buf_height - 1);
	int highy = mg_clamp(MG_MAX(y1, y2), 0, (int)mg.buf_height - 1);

	for (; lowy <= highy; ++lowy)
		for (x = lowx; x <= highx; ++x)
			MG_PIXELSET_NOBOUNDSCHECK(x, lowy);
}

void
mg_filltriangle(int x1, int y1, int x2, int y2, int x3, int y3)
{
	/*
	 * see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
	 * for the algorithm used here
	 */
	int nx1, ny1, nx2, ny2, nx3, ny3;
	mg_sort_ascending_by_y(x1, y1, x2, y2, x3, y3, &nx1, &ny1, &nx2, &ny2, &nx3, &ny3);
	/* now we can assume that ny1 <= ny2 <= ny3 */

	if (ny2 == ny3) {
		/* bottom-flat triangle */
		mg_fillflatsidetriangle(nx1, ny1, nx2, ny2, nx3, ny3);
	} else if (ny1 == ny2) {
		/* top-flat triangle */
		mg_fillflatsidetriangle(nx3, ny3, nx1, ny1, nx2, ny2);
	} else {
		/* split triangle into top-flat && bottom-flat */
		int tmp = (int)((float)nx1 + ((float)(ny2 - ny1) / (float)(ny3 - ny1)) * (float)(nx3 - nx1));
		mg_fillflatsidetriangle(nx1, ny1, nx2, ny2, tmp, ny2);
		mg_fillflatsidetriangle(nx3, ny3, nx2, ny2, tmp, ny2);
	}
}

void
mg_flush(void)
{
	/* submit a frame for this event */
	if (!mg.closed) {
		struct wl_buffer *buffer = mg_draw_frame(&mg);
		wl_surface_attach(mg.wl_surface, buffer, 0, 0);
		wl_surface_damage_buffer(mg.wl_surface, 0, 0, INT32_MAX, INT32_MAX);
		wl_surface_commit(mg.wl_surface);
	}
}

/* functions to change options */
void
mg_setbgcolor(uint8_t r, uint8_t g, uint8_t b)
{
	/* remember we're using XRGB */
	mg.bgcolor = (uint32_t)((r << 16) | (g << 8) | b);
}

void
mg_setdrawcolor(uint8_t r, uint8_t g, uint8_t b)
{
	/* remember we're using XRGB */
	mg.color = (uint32_t)((r << 16) | (g << 8) | b);
}

/* events */
void
mg_waitevent(struct mg_event *event)
{
	for (;;) {
		/* wait for a wayland event to happen */
		wl_display_dispatch(mg.wl_display);

		/* handle event */
		if (mg_handle_wl_event(event))
			break;
	}
}

/* initialization & shutdown */
int
mg_init(int w, int h, const char *title)
{
	mg.closed = 0;
	mg.bgcolor = 0x00000000;
	mg.color = 0x00FFFFFF;

	mg_width = w;
	mg_height = h;
	mg.buf_width = (size_t)mg_width;
	mg.buf_height = (size_t)mg_height;
	mg.buf_stride = mg.buf_width * 4;
	mg.buf_size = mg.buf_stride * mg.buf_height;

	if (!(mg.draw_buf = calloc(mg.buf_size, 1)))
		return -1;

	mg.pending_quit = 0;
	mg.pending_resize = 0;
	mg.pending_key_down = 0;
	mg.pending_key_up = 0;
	mg.pending_mouse_down.type = MG_NOEVENT;
	mg.pending_mouse_up.type = MG_NOEVENT;
	mg.pending_mouse_motion.type = MG_NOEVENT;

	mg.wl_display = wl_display_connect(NULL);
	mg.wl_registry = wl_display_get_registry(mg.wl_display);
	mg.xkb_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
	wl_registry_add_listener(mg.wl_registry, &wl_registry_listener, &mg);
	wl_display_roundtrip(mg.wl_display);

	mg.wl_surface = wl_compositor_create_surface(mg.wl_compositor);
	mg.xdg_surface = xdg_wm_base_get_xdg_surface(
			mg.xdg_wm_base, mg.wl_surface);
	xdg_surface_add_listener(mg.xdg_surface, &mg_xdg_surface_listener, &mg);
	mg.xdg_toplevel = xdg_surface_get_toplevel(mg.xdg_surface);
	xdg_toplevel_add_listener(mg.xdg_toplevel,
			&mg_xdg_toplevel_listener, &mg);
	xdg_toplevel_set_title(mg.xdg_toplevel, title);
	wl_surface_commit(mg.wl_surface);

	return 0;
}

void
mg_quit(void)
{
	if (!mg.closed) {
		free(mg.draw_buf);
		mg.draw_buf = NULL;

		wl_surface_attach(mg.wl_surface, NULL, 0, 0);
		wl_surface_commit(mg.wl_surface);
		wl_surface_destroy(mg.wl_surface);
		xdg_surface_destroy(mg.xdg_surface);
		xdg_toplevel_destroy(mg.xdg_toplevel);
		xdg_wm_base_destroy(mg.xdg_wm_base);
		wl_display_disconnect(mg.wl_display);
		mg.closed = 1;
	}
}
#endif /* defined(MG_BACKEND_WAYLAND) */

#endif /* MG_IMPLEMENTATION */
