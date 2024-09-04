an example program to draw a bunch of shapes (rectangles, triangles, circles) and have them move across the screen. the W/S keys can be used to move them up/down.

compile with one of the following:
```
make         # compiles both X11 and Wayland versions
make x11     # compiles X11 version only
make wayland # compiles Wayland version only
```

and run it with one of the following:
```
./moving-shapes-x11     # run X11 version
./moving-shapes-wayland # run Wayland version
```

run `make clean` to remove the binaries and Wayland protocol files.
