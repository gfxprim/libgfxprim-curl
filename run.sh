#!/bin/sh

BIN="$1"
shift

LD_LIBRARY_PATH=$PWD:../gfxprim/build/ ./$BIN $@
