#!/bin/sh -xe

CC=clang
CFLAGS="-Wall -Wextra -Werror"
CLIBS="-I./src/ -lX11"

NAME=vcc
SOURCE="vc.c"

$CC $CFLAGS -o ./$NAME $SOURCE $CLIBS && \
./$NAME
