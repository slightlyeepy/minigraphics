an example program to draw some text saying "hello world".

compile with one of the following:
```
make         # compiles both X11 and Wayland versions
make x11     # compiles X11 version only
make wayland # compiles Wayland version only
```

and run it with one of the following:
```
./hello-world-x11     # run X11 version
./hello-world-wayland # run Wayland version
```

run `make clean` to remove the binaries and Wayland protocol files.
