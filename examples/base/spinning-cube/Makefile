.POSIX:

CC      = cc
SRC     = spinning-cube.c
HEADERS = ../../../minigraphics.h

WFLAGS  = -std=c99 -Werror -pedantic -Wall -Wextra
CFLAGS  = -lrt -lm -I. -I../../.. -D_POSIX_C_SOURCE=200112L -Ofast $(WFLAGS)

X11_CFLAGS  = -lX11 -DMG_BACKEND_X11
WL_CFLAGS   = -lwayland-client -lxkbcommon -DMG_BACKEND_WAYLAND
WL_PROTOCOL = xdg-shell-protocol.c xdg-shell-client-protocol.h

all: x11 wayland

x11: $(SRC) $(HEADERS)
	$(CC) $(X11_CFLAGS) $(CFLAGS) -o spinning-cube-x11 $(SRC)

wayland: $(SRC) $(HEADERS) $(WL_PROTOCOL)
	$(CC) $(WL_CFLAGS) $(CFLAGS) -o spinning-cube-wayland xdg-shell-protocol.c $(SRC)
xdg-shell-protocol.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner private-code \
		< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
		> xdg-shell-protocol.c
xdg-shell-client-protocol.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header \
		< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
		> xdg-shell-client-protocol.h

clean:
	rm -f spinning-cube-x11 spinning-cube-wayland $(WL_PROTOCOL) *.o
