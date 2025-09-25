minigraphics is a small single-header library for drawing stuff from an array of pixels to a window, and receiving user input from that window.

current features include:
- X11 and Wayland backends
- SDL-like event handling loop
- drawing from a memory buffer

# usage

drop [minigraphics.h](minigraphics.h) into your project.

if you don't like the single-header format, you can create a file `minigraphics.c` with the contents:
```c
#define MG_IMPLEMENTATION
#include "minigraphics.h"
```
and use `minigraphics.c` and `minigraphics.h` as a standard source file / header pair.

you can also install it with [clib](https://github.com/clibs/clib):

```sh
clib install slightlyeepy/minigraphics
```

you won't need any code modifications for different backends, however you'll need some build system setup. see the top comment in [the header](minigraphics.h) (section `BACKEND-SPECIFIC INSTRUCTIONS`) for more info, and/or the makefiles in [examples](examples) for an example.

# license

your choice between public domain (Unlicense) or MIT, see [the header](minigraphics.h) for more info.

# docs

the main documentation is located in [the header](minigraphics.h) itself as comments; you might also want to see [the example programs](examples). examples are provided for using the library "raw" and in conjunction with [olive.c](https://github.com/tsoding/olive.c).

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
- more backends. the two i'm currently looking at are linux framebuffer and UEFI GOP. a windows GDI backend would be very useful but gahh i really don't wanna deal with windows
