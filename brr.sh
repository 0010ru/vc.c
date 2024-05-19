#!/bin/sh -xe

CC=clang
CFLAGS="-Wall -Wextra -Werror"

mkdir -p ./dst/

$CC $CFLAGS -c -fPIC -o ./dst/vcc.o ./src/vc.c
$CC $CFLAGS -c -fPIC -o ./dst/vcm.o ./src/vcm.c
