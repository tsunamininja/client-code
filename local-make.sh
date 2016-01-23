#!/bin/bash

CCFLAGS="-std=c99 -ggdb -D_POSIX_C_SOURCE=2 -mpreferred-stack-boundary=2"
SOURCE="main.c dns-protocol.c bufferHelper.c"
EXE=myprog

# compile and link
gcc $CCFLAGS -o $EXE $SOURCE
