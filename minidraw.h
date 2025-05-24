/*
 * minidraw in-development (versioned releases will come later)
 *
 * NOTE: minidraw uses identifiers beginning with 'mgd__' or 'MGD__'
 * internally; using these may cause conflicts.
 *
 * ===========================================================================
 * LICENSE
 *
 * see the end of the file for license information.
 *
 * ===========================================================================
 * USAGE
 *
 * include this header wherever you need it as usual. in ONE file, add:
 * 	#define MGD_IMPLEMENTATION
 * before the #include for this header.
 *
 * to make the implementation private to the file that generates it, also add:
 * 	#define MGD_STATIC
 *
 * see the "DOCS" section for the API documentation.
 * see the examples directory for example programs.
 */
#if !defined(MGD_H)
#define MGD_H

#if defined(MGD_STATIC)
#define MGD__DEF static
#else
#define MGD__DEF extern
#endif /* defined(MGD_STATIC) */

/*
 * ===========================================================================
 * DOCS (+ header)
 */
/* draw buffer struct. */
struct draw {
	uint32_t *data;
	uint32_t width, height;
#if defined(MG_H)
	enum mg_pixel_format pixel_format;
#else
	int pixel_format;
#endif /* defined(MG_H) */
};

/*
 * convenience func to resize the draw buffer without distorting the image.
 * returns 1 if resizing failed due to out-of-memory, and 0 otherwise.
 */
MGD__DEF int mgd_image_resize(struct draw *draw, uint32_t width, uint32_t height);

/*
 * all of these functions take the buffer to which to draw to ('draw'),
 * and the pixel with which to draw ('pixel').
 */

/*
 * draw an outline of a circle with its middle point being located
 * at (x, y) and its radius being 'r'.
 */
MGD__DEF void mgd_drawcircle(const struct draw *draw, uint32_t pixel,
		int x, int y, int r);

/* draw a line from the point (x1, y1) to the point (x2, y2). */
MGD__DEF void mgd_drawline(const struct draw *draw, uint32_t pixel,
		int x1, int y1, int x2, int y2);

/* draw a pixel at the point (x, y). */
MGD__DEF void mgd_drawpixel(const struct draw *draw, uint32_t pixel,
		int x, int y);

/*
 * draw an outline of a rectangle with its top-left point being located
 * at (x1, y1) and its bottom-left point being at (x2, y2).
 */
MGD__DEF void mgd_drawrect(const struct draw *draw, uint32_t pixel,
		int x1, int y1, int x2, int y2);

/*
 * draw a string in an 8x8 bitmap font, with the top-left corner of the
 * first 8x8 character box being at (x, y). the 'size' parameter specifies
 * the font size, so a 'size' of 2 will draw 16x16 text.
 */
MGD__DEF void mgd_drawtext(const struct draw *draw, uint32_t pixel,
		int x, int y, const char *text, int size);

/*
 * draw an outline of a triangle with its vertices being at the points
 * (x1, y1), (x2, y2), and (x3, y3).
 */
MGD__DEF void mgd_drawtriangle(const struct draw *draw, uint32_t pixel,
		int x1, int y1, int x2, int y2, int x3, int y3);

/* fill the whole buffer with a color. */
MGD__DEF void mgd_fill(const struct draw *draw, uint32_t pixel);

/*
 * draw a filled-in circle with its middle point being located at (x, y)
 * and its radius being 'r'.
 */
MGD__DEF void mgd_fillcircle(const struct draw *draw, uint32_t pixel,
		int x, int y, int r);

/*
 * draw a filled-in rectangle with its top-left point being located at
 * (x1, y1) and its bottom-left point being at (x2, y2).
 */
MGD__DEF void mgd_fillrect(const struct draw *draw, uint32_t pixel,
		int x1, int y1, int x2, int y2);

/*
 * draw a filled-in triangle with its vertices being at the points (x1, y1),
 * (x2, y2), and (x3, y3).
 */
MGD__DEF void mgd_filltriangle(const struct draw *draw, uint32_t pixel,
		int x1, int y1, int x2, int y2, int x3, int y3);

/* end of docs/header */

#endif /* !defined(MGD_H) */

/*
 * ===========================================================================
 * IMPLEMENTATION
 */
#if defined(MGD_IMPLEMENTATION)

/* constants */
static const unsigned char mgd__font[95][8] = {
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

/* macros */
#define MGD__MIN(x, y) (((x) < (y)) ? (x) : (y))
#define MGD__MAX(x, y) (((x) > (y)) ? (x) : (y))

/* 3 == MG_PIXEL_FORMAT_256 */
#define MGD__PIXELSET(draw, pixel, x, y) \
	if ((x) >= 0 && (y) >= 0 && (x) < (int)draw->width && (y) < (int)draw->height) { \
		if (draw->pixel_format != 3) \
			draw->data[(y) * (int)draw->width + (x)] = pixel; \
		else \
			((uint8_t *)draw->data)[(y) * (int)draw->width + (x)] = (uint8_t)pixel; \
	}
#define MGD__PIXELSET_NOBOUNDSCHECK(draw, pixel, x, y) \
	if (draw->pixel_format != 3) \
		draw->data[(y) * (int)draw->width + (x)] = pixel; \
	else \
		((uint8_t *)draw->data)[(y) * (int)draw->width + (x)] = (uint8_t)pixel;

/* internal functions */
static int
mgd__clamp(int val, int min, int max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;
	return val;
}

static void
mgd__fillflatsidetriangle(const struct draw *draw, uint32_t pixel, int x1, int y1, int x2, int y2, int x3, int y3)
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

	int i = 0, tmp;
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

	for (; i <= dx1; ++i) {
		/* mgd_drawline(tmp1_x, tmp1_y, tmp2_x, tmp2_y); */

		if (tmp1_y >= 0 && tmp1_y < (int)draw->height) {
			/* assuming tmp1_y == tmp2_y */
			lowx = mgd__clamp(MGD__MIN(tmp1_x, tmp2_x), 0, (int)draw->width - 1);
			highx = mgd__clamp(MGD__MAX(tmp1_x, tmp2_x), 0, (int)draw->width - 1);
			for (; lowx <= highx; ++lowx)
				MGD__PIXELSET_NOBOUNDSCHECK(draw, pixel, lowx, tmp1_y)
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
mgd__sort_ascending_by_y(int x1, int y1, int x2, int y2, int x3, int y3,
		int *nx1, int *ny1, int *nx2, int *ny2, int *nx3, int *ny3)
{
#define MGD__SWAP(x, y) tmp = x; x = y; y = tmp;
	int tmp;
	if (y1 > y3) {
		MGD__SWAP(y1, y3);
		MGD__SWAP(x1, x3);
	}
	if (y1 > y2) {
		MGD__SWAP(y1, y2);
		MGD__SWAP(x1, x2);
	}
	if (y2 > y3) {
		MGD__SWAP(y2, y3);
		MGD__SWAP(x2, x3);
	}
#undef MGD__SWAP

	/* remember that lower Y value = higher */
	*nx1 = x3;
	*ny1 = y3;
	*nx2 = x2;
	*ny2 = y2;
	*nx3 = x1;
	*ny3 = y1;
}

/* public functions */
int
mgd_image_resize(struct draw *draw, uint32_t width, uint32_t height)
{
	/* 3 == MG_PIXEL_FORMAT_256 */
	uint32_t *newdata;
	size_t roff = 0, woff = 0;
	size_t stride;
	size_t i = 0;
	if (draw->pixel_format != 3) {
		newdata = calloc(width * height, sizeof(uint32_t));
		stride = MGD__MIN(draw->width * sizeof(uint32_t), width * sizeof(uint32_t));

		if (!newdata)
			return 1;

		for (; i < MGD__MIN(draw->height, height) &&
				(roff < draw->width * draw->height) &&
				(woff < width * height); ++i) {
			memcpy(newdata + woff, draw->data + roff, stride);
			roff += draw->width;
			woff += width;
		}
	} else {
		uint8_t *p = malloc(width * height);
		stride = MGD__MIN(draw->width, width);

		if (!p)
			return 1;

		for (; i < MGD__MIN(draw->height, height) &&
				(roff < draw->width * draw->height) &&
				(woff < width * height); ++i) {
			memcpy(p + woff, draw->data + roff, stride);
			roff += draw->width;
			woff += width;
		}
		newdata = (uint32_t *)p;
	}
	free(draw->data);
	draw->data = newdata;
	draw->width = width;
	draw->height = height;
	return 0;
}

void
mgd_drawcircle(const struct draw *draw, uint32_t pixel, int x, int y, int r)
{
	int dx = -r, dy = 0, err = 2 - 2 * r;
	do {
		MGD__PIXELSET(draw, pixel, x - dx, y + dy)
		MGD__PIXELSET(draw, pixel, x - dy, y - dx)
		MGD__PIXELSET(draw, pixel, x + dx, y - dy)
		MGD__PIXELSET(draw, pixel, x + dy, y + dx)

		r = err;
		if (r <= dy)
			err += ++dy * 2 + 1;
		if (r > dx || err > dy)
			err += ++dx * 2 + 1;
	} while (dx < 0);
}

void
mgd_drawline(const struct draw *draw, uint32_t pixel, int x1, int y1, int x2, int y2)
{
	x1 = mgd__clamp(x1, 0, (int)draw->width - 1);
	y1 = mgd__clamp(y1, 0, (int)draw->height - 1);
	x2 = mgd__clamp(x2, 0, (int)draw->width - 1);
	y2 = mgd__clamp(y2, 0, (int)draw->height - 1);
	if (x1 == x2) {
		int lowy = MGD__MIN(y1, y2);
		int highy = MGD__MAX(y1, y2);
		for (; lowy <= highy; ++lowy)
			MGD__PIXELSET_NOBOUNDSCHECK(draw, pixel, x1, lowy)
	} else if (y1 == y2) {
		int lowx = MGD__MIN(x1, x2);
		int highx = MGD__MAX(x1, x2);
		for (; lowx <= highx; ++lowx)
			MGD__PIXELSET_NOBOUNDSCHECK(draw, pixel, lowx, y1)
	} else {
		int dx = abs(x2 - x1), sx = (x1 < x2) ? 1 : -1;
		int dy = -abs(y2 - y1), sy = (y1 < y2) ? 1 : -1;
		int err = dx + dy, e2;

		for (;;) {
			MGD__PIXELSET_NOBOUNDSCHECK(draw, pixel, x1, y1)
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
mgd_drawpixel(const struct draw *draw, uint32_t pixel, int x, int y)
{
	MGD__PIXELSET(draw, pixel, x, y)
}

void
mgd_drawrect(const struct draw *draw, uint32_t pixel, int x1, int y1, int x2, int y2)
{
	mgd_drawline(draw, pixel, x1, y1, x2, y1); /* top */
	mgd_drawline(draw, pixel, x2, y1, x2, y2); /* right */
	mgd_drawline(draw, pixel, x1, y2, x2, y2); /* bottom */
	mgd_drawline(draw, pixel, x1, y1, x1, y2); /* left */
}

void
mgd_drawtext(const struct draw *draw, uint32_t pixel, int x, int y, const char *text, int size)
{
	if (size < 1) {
		return;
	} else {
		size_t i = 0;
		int px, py; /* point x, point y */
		int dx = x; /* draw->data x */

		for (; i < strlen(text); ++i) {
			for (py = 0; py < 8; ++py) {
				for (px = 0; px < 8; ++px) {
					if (text[i] >= 0x20 && text[i] <= 0x7e &&
							(mgd__font[text[i] - 0x20][py] & 1 << px)) {
						int tmpx = dx + (px * size);
						int tmpy = y + (py * size);
						if (size > 1)
							mgd_fillrect(draw, pixel, tmpx, tmpy,
									tmpx + size - 1, tmpy + size - 1);
						else
							MGD__PIXELSET(draw, pixel, tmpx, tmpy)
					}
				}
			}
			dx += 8 * size;
		}
	}
}

void
mgd_drawtriangle(const struct draw *draw, uint32_t pixel, int x1, int y1, int x2, int y2, int x3, int y3)
{
	mgd_drawline(draw, pixel, x1, y1, x2, y2);
	mgd_drawline(draw, pixel, x2, y2, x3, y3);
	mgd_drawline(draw, pixel, x3, y3, x1, y1);
}

void
mgd_fill(const struct draw *draw, uint32_t pixel)
{
	size_t i = 0;
	/* 3 == MG_PIXEL_FORMAT_256 */
	for (; i < draw->width * draw->height; ++i) {
		if (draw->pixel_format != 3)
			draw->data[i] = pixel;
		else
			((uint8_t *)draw->data)[i] = (uint8_t)pixel;
	}
}

void
mgd_fillcircle(const struct draw *draw, uint32_t pixel, int x, int y, int r)
{
	/* TODO: this is really bad.... */
	int dx, dy = -r;
	for (; dy <= r; ++dy)
		for (dx = -r; dx <= r; ++dx)
			if (dx * dx + dy * dy < r * r + r)
				MGD__PIXELSET(draw, pixel, dx + x, dy + y)
}

void
mgd_fillrect(const struct draw *draw, uint32_t pixel, int x1, int y1, int x2, int y2)
{
	int x;
	int lowx = mgd__clamp(MGD__MIN(x1, x2), 0, (int)draw->width - 1);
	int highx = mgd__clamp(MGD__MAX(x1, x2), 0, (int)draw->width - 1);
	int lowy = mgd__clamp(MGD__MIN(y1, y2), 0, (int)draw->height - 1);
	int highy = mgd__clamp(MGD__MAX(y1, y2), 0, (int)draw->height - 1);

	for (; lowy <= highy; ++lowy)
		for (x = lowx; x <= highx; ++x)
			MGD__PIXELSET_NOBOUNDSCHECK(draw, pixel, x, lowy)
}

void
mgd_filltriangle(const struct draw *draw, uint32_t pixel, int x1, int y1, int x2, int y2, int x3, int y3)
{
	/*
	 * see http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
	 * for the algorithm used here
	 */
	int nx1, ny1, nx2, ny2, nx3, ny3;
	mgd__sort_ascending_by_y(x1, y1, x2, y2, x3, y3, &nx1, &ny1, &nx2, &ny2, &nx3, &ny3);
	/* now we can assume that ny1 <= ny2 <= ny3 */

	if (ny2 == ny3) {
		/* bottom-flat triangle */
		mgd__fillflatsidetriangle(draw, pixel, nx1, ny1, nx2, ny2, nx3, ny3);
	} else if (ny1 == ny2) {
		/* top-flat triangle */
		mgd__fillflatsidetriangle(draw, pixel, nx3, ny3, nx1, ny1, nx2, ny2);
	} else {
		/* split triangle into top-flat && bottom-flat */
		int tmp = (int)((float)nx1 + ((float)(ny2 - ny1) / (float)(ny3 - ny1)) * (float)(nx3 - nx1));
		mgd__fillflatsidetriangle(draw, pixel, nx1, ny1, nx2, ny2, tmp, ny2);
		mgd__fillflatsidetriangle(draw, pixel, nx3, ny3, nx2, ny2, tmp, ny2);
	}
}

#endif /* MGD_IMPLEMENTATION */

/*
 * This library is available under 2 licenses - choose whichever you prefer.
 *
 * ---------------------------------------------------------------------------
 * ALTERNATIVE A - Unlicense
 *
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
 *
 * ---------------------------------------------------------------------------
 * ALTERNATIVE B - MIT/X Consortium License
 *
 * Copyright (c) 2025 slightlyeepy
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
