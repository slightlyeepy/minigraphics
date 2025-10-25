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
- POSIX.1-2001 or later compliant standard realtime library (`-lrt`)
- libxkbcommon
- libwayland + wayland-protocols

# goals

- simple & easy to use API
- small C99-compatible codebase
- allow the same code to work on different backends with no code modifications

# non-goals

- fancier drawing features
- hardware acceleration, etc.

# todo

high priority:
- for library errors that internally occurred cause some function failed and set `errno`, preserve that errno value and provide a `strerror()` of it in `mg_strerror()`. also consider renaming `mg_errno` -> `mg_errstatus` and `mg_strerror()` ->` mg_errstring()`
- `mg_waitevent()` with a timeout, like SDL's `SDL_WaitEventTimeout()`
- something better for keyboard input. ideally a keypress should be a scancode/keysym pair like in SDL. also this may be unviable but not depending on the libxkbcommon header would be nice
- timestamps for events and something to get the current timestamp similar to `SDL_GetTicks()`
- mouse wheel events. right now there's some useless code in the Wayland backend for handling axis events, it should go to use
- some equivalent to `SDL_Delay()`, right now there isn't a good way to sleep without freezing the entire process

low priority:
- documentation for the internals of the library
- more backends. the two i'm currently looking at are linux framebuffer and UEFI GOP. a windows GDI backend would be very useful but gahh i really don't wanna deal with windows
- text input api
- initially performance wasn't a goal, but maybe DO think about performance? the premise of the library is very simple and it'd suck if something like this was unusable as a little "display stuff on the screen" backend purely because of performance issues
