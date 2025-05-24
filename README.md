minigraphics is a small single-header library for drawing stuff from an array of pixels to a window, and receiving user input from that window.

current features include:
- X11 and Wayland backends
- SDL-like event handling loop
- drawing from a memory buffer

an [additional utility library](minidraw.h) exists for:
- drawing pixels/lines/triangles/rectangles/circles, both filled and outlined
- drawing ASCII text in an 8x8 bitmap font

# usage

install with [clib](https://github.com/clibs/clib):

```sh
clib install slightlyeepy/minigraphics
```

or just drop [minigraphics.h](minigraphics.h) and/or [minidraw.h](minidraw.h) inside your project.

# license

your choice between public domain (Unlicense) or MIT, see [the header](minigraphics.h) for more info. same goes for `minidraw`.

# docs

the main documentation is located in [the header](minigraphics.h) itself as comments; you might also want to see [the example programs](examples).

# dependencies

- POSIX.1-2001 or later compliant standard C library
- `<xkbcommon/xkbcommon-keysyms.h>` libxkbcommon header

X11 only:
- libX11

Wayland only:
- libxkbcommon
- libwayland + wayland-protocols
- POSIX.1-2001 or later compliant standard realtime library (`-lrt`)

# goals

- simple / easy to use API
- small C89-compatible codebase
- allow the same code to work on different backends with no code modifications

# non-goals

- high performance / usage in places where performance is highly important
- hardware acceleration, etc.

# todo

- support more features like unresizable windows, etc.
- linux framebuffer backend
- windows GDI backend (maybe?)
