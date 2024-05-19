#!/bin/sh -xe

CC=clang
CFLAGS="-Wall -Wextra -Werror"

$CC $CFLAGS -c -fPIC -o vcc.o vc.c
$CC $CFLAGS -c -fPIC -o vcm.o vcm.c