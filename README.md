minigraphics is a small single-header library for drawing primitive shapes and text, providing a simple abstraction layer over multiple platforms (kind of like a smaller, more barebones SDL).

current features include:
- X11 and Wayland backends
- SDL-like event handling loop
- drawing from a memory buffer
- drawing pixels/lines/triangles/rectangles/circles, both filled and outlined
- drawing ASCII text in an 8x8 bitmap font

minigraphics provides two versions: a "small" one that can only draw things from a memory buffer, and a "regular" one that additionally provides an interface to draw shapes and text.

# license

your choice between MIT/public domain, see [the header](minigraphics.h) for more info.

# docs

the main documentation is located in [the header](minigraphics.h) ([small edition](sminigraphics.h)) itself as comments; you might also want to see [the example programs](examples).

# dependencies

- POSIX.1-2001 or later compliant standard C library
- libxkbcommon
- libX11 (X11 backend only)
- libwayland + wayland-protocols (Wayland backend only)
- POSIX.1-2001 or later compliant standard realtime library (`-lrt`) (Wayland backend only)

# goals

- simple / easy to use API
- small C89-compatible codebase
- allow the same code to work on different backends with no modifications

# non-goals

- high performance / usage in places where performance is highly important
- hardware acceleration, etc.

# todo

- linux framebuffer backend
- use frame callbacks on wayland
- windows GDI backend (maybe?)
