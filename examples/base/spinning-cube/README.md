quick-and-dirty 3d render of a spinning cube (modification of [cube.c](https://github.com/servetgulnaroglu/cube.c))

compile with one of the following:
```
make         # compiles both X11 and Wayland versions
make x11     # compiles X11 version only
make wayland # compiles Wayland version only
```

and run it with one of the following:
```
./spinning-cube-x11     # run X11 version
./spinning-cube-wayland # run Wayland version
```

run `make clean` to remove the binaries and Wayland protocol files.
