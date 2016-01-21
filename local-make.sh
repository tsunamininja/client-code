#!/bin/bash

CCFLAGS="-std=c99 -ggdb -D_POSIX_C_SOURCE=2 -mpreferred-stack-boundary=2"
SOURCE="*.c"
EXE=myprog

# compile and link
gcc $CCFLAGS -o $EXE $SOURCE
