#!/bin/sh -xe

CC=clang
CFLAGS="-Wall -Wextra -Werror"
CLIBS="-lX11"

NAME=vcc
SOURCE="vc.c"

$CC $CFLAGS -o ./$NAME $SOURCE $CLIBS && \
./$NAME
