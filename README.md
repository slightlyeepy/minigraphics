minigraphics is a small single-header library for drawing primitive shapes and text, providing a simple abstraction layer over multiple platforms (kind of like a smaller, more barebones SDL).

current features include:
- X11 and Wayland backends
- drawing pixels/lines/triangles/rectangles/circles, both filled and outlined
- drawing ASCII text in an 8x8 bitmap font
- SDL-like event handling loop

# license

your choice between MIT/public domain, see [the header](minigraphics.h) for more info.

# docs

the main documentation is located in [the header](minigraphics.h) itself as comments; you might also want to see [the example program](example/example.c) for a practical example for a program/build system.

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
