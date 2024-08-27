minigraphics is a small single-header library for drawing primitive shapes and text, providing an abstraction layer over several platforms such as X11 and Wayland.

# goals

- simple / easy to use API
- small codebase
- allow the same code to work on different backends with no modifications

# non-goals

- high performance
- hardware acceleration, etc.

# todo

- implement non-blocking event handling
- use frame callbacks on wayland
- windows GDI backend (maybe?)
