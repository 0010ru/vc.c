#!/bin/sh -xe

EDITOR=hx
CC=clang
CFLAGS="-Wall -Wextra -Werror"
CLIBS="-lX11"

NAME=vox
SOURCE=./src/main.c

$EDITOR $SOURCE && \
$CC $CFLAGS -o ./$NAME $SOURCE $CLIBS && \
./$NAME
