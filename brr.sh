#!/bin/sh -xe

# CC="clang"
# CFLAGS="-Wall -Wextra -Werror -c -fPIC"
# LDFLAGS="-lX11"

gcc -o main src/main.c src/vc_arena.c src/vc_core.c -lX11 && ./main
