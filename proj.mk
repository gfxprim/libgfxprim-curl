CFLAGS+=$(shell gfxprim-config --cflags) $(shell curl-config --cflags)
LIB=gfxprim-curl
LIB_SRCS=gp_dialog_download.c
LIB_HEADERS=$(wildcard *.h)
LIB_LDLIBS=$(shell curl-config --libs)

BIN_INSTALL=0
BIN=download_example
LDLIBS=$(shell gfxprim-config --libs-widgets --libs)
