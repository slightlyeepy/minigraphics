an example program to log all recieved events to stdout.

compile with one of the following:
```
make         # compiles both X11 and Wayland versions
make x11     # compiles X11 version only
make wayland # compiles Wayland version only
```

and run it with one of the following:
```
./events-x11     # run X11 version
./events-wayland # run Wayland version
```

run `make clean` to remove the binaries and Wayland protocol files.

depends on libxkbcommon, even on the X11 version.
