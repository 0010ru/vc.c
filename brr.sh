#!/bin/sh -xe

NAME="vc"
CC="clang"
CFLAGS="-Wall -Wextra -Werror -ggdb -c -fPIC"
LDFLAGS="-lX11"
SOURCE_DIR="src"
INCLUDE_DIR="include"
DESTINATION_DIR="lib"

mkdir -p $DESTINATION_DIR

VCL="core"
VCL_C="${SOURCE_DIR}/${NAME}_${VCL}.c"
VCL_O_0="${DESTINATION_DIR}/${NAME}_${VCL}.o"
$CC $CFLAGS -o $VCL_O_0 $VCL_C

VCL="arena"
VCL_C="${SOURCE_DIR}/${NAME}_${VCL}.c"
VCL_O_1="${DESTINATION_DIR}/${NAME}_${VCL}.o"
$CC $CFLAGS -o $VCL_O_1 $VCL_C

LIB_OUT="${DESTINATION_DIR}/lib${NAME}.a"
ar rcs $LIB_OUT $VCL_O_0 $VCL_O_1
rm $VCL_O_0 $VCL_O_1