#!/bin/sh -xe

NAME=vc
CC=clang
CFLAGS="-Wall -Wextra -Werror -ggdb -c -fPIC"
LDFLAGS="-lX11"
SOURCE_DIR=src
INCLUDE_DIR=include
DESTINATION_DIR=lib

VCC_C=$SOURCE_DIR/vc_core.c
VCC_O=$DESTINATION_DIR/vc_core.o
VCM_C=$SOURCE_DIR/vc_arena.c
VCM_O=$DESTINATION_DIR/vc_arena.o

mkdir -p $DESTINATION_DIR
$CC $CFLAGS -o $VCC_O $VCC_C
$CC $CFLAGS -o $VCM_O $VCM_C
ar rcs $DESTINATION_DIR/lib$NAME.a $VCC_O $VCM_O
rm $VCC_O $VCM_O