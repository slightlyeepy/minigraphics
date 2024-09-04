a very simple [farbfeld](https://tools.suckless.org/farbfeld/) image viewer.

note: there are literally no features besides displaying the image (even zooming in/out), so you probably shouldn't use this as an actual image viewer, it's just a little demo

supported formats:
- farbfeld (normal, bzip2, gzip, xz)
- PNG
- JPG

compile with one of the following:
```
make         # compiles both X11 and Wayland versions
make x11     # compiles X11 version only
make wayland # compiles Wayland version only
```

and run it with:
```
./image-view.sh <image>
```

if both `farbfeld-view-x11` and `farbfeld-view-wayland` executables are present, the X11 version is used.

optional dependencies:
- farbfeld tools (for viewing PNG/JPG images)
- bzip2 (for viewing bzip2-compressed farbfeld images)
- gzip (for viewing gzip-compressed farbfeld images)
- xz (for viewing xz-compressed farbfeld images)
