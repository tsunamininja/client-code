#!/bin/bash

BUFFER_HELPER_LOCATION=/home/joe/workbench/dns-client/bufferHelper.c

gcc -ggdb -mpreferred-stack-boundary=2 -o main \
main.c dns-protocol.c $BUFFER_HELPER_LOCATION \
-std=c99 -D_POSIX_C_SOURCE=2
