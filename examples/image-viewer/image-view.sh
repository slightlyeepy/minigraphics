#!/bin/sh
if [ -z "$1" ]; then
	echo "usage: $0 image" >&2
	exit 1
fi

filename="$1"
shift
ext=$(basename "$filename" | grep -o '\..[^\.]*$')
ff_view=''

if [ -f './farbfeld-view-x11' ]; then
	ff_view='./farbfeld-view-x11'
elif [ -f './farbfeld-view-wayland' ]; then
	ff_view='./farbfeld-view-wayland'
else
	echo 'farbfeld-view-x11/farbfeld-view-wayland executable not found' >&2
	exit 1
fi

check_installed() {
	if ! which "$1" > /dev/null 2>&1; then
		echo "it looks like you don't have $1." >&2
		echo "you should install $2." >&2
		exit 1
	fi
}

ffmsg="your distro's 'farbfeld' package if it exists, or otherwise get it from https://tools.suckless.org/farbfeld/"
case "$ext" in
	".jpg") check_installed jpg2ff "$ffmsg"; jpg2ff < "$filename" | "$ff_view" - ;;
	".png") check_installed png2ff "$ffmsg"; png2ff < "$filename" | "$ff_view" - ;;
	".ff") "$ff_view" "$filename" ;;
	".ff.bz2") check_installed bzip2 "your distro's 'bzip2' package"; bzip2 -dc "$filename" | "$ff_view" - ;;
	".ff.gz") check_installed gzip "your distro's 'gzip' package"; gzip -dc "$filename" | "$ff_view" - ;;
	".ff.xz") check_installed xz "your distro's 'xz' package"; xz -dc "$filename" | "$ff_view" - ;;
	*) echo "unknown file format '$ext'" >&2; exit 1 ;;
esac
