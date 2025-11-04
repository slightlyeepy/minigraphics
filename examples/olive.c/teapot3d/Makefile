.POSIX:

CC       = cc
SRC      = teapot3d.c
HEADERS  = ../../../minigraphics.h ../olive.c
RAWMODEL = utahTeapot.c
MODEL    = utahTeapot.c.xz

OLIVEC_WFLAGS=-Wno-missing-braces -Wno-strict-aliasing -Wno-unused-variable
WFLAGS  = -std=c99 -Werror -pedantic -Wall -Wextra $(OLIVEC_WFLAGS)
CFLAGS  = -lrt -I. -I.. -I../../.. -D_POSIX_C_SOURCE=200112L -Ofast -lm $(WFLAGS)

X11_CFLAGS  = -lX11 -DMG_BACKEND_X11
WL_CFLAGS   = -lwayland-client -lxkbcommon -DMG_BACKEND_WAYLAND
WL_PROTOCOL = xdg-shell-protocol.c xdg-shell-client-protocol.h

all: x11 wayland

$(RAWMODEL): $(MODEL)
	xzcat $(MODEL) > $(RAWMODEL)

x11: $(SRC) $(HEADERS) $(RAWMODEL)
	$(CC) $(X11_CFLAGS) $(CFLAGS) -o teapot3d-x11 $(SRC)

wayland: $(SRC) $(HEADERS) $(RAWMODEL) $(WL_PROTOCOL)
	$(CC) $(WL_CFLAGS) $(CFLAGS) -o teapot3d-wayland xdg-shell-protocol.c $(SRC)
xdg-shell-protocol.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner private-code \
		< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
		> xdg-shell-protocol.c
xdg-shell-client-protocol.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header \
		< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
		> xdg-shell-client-protocol.h

clean:
	rm -f teapot3d-x11 teapot3d-wayland $(RAWMODEL) $(WL_PROTOCOL) *.o
