CFLAGS+=$(shell gfxprim-config --cflags) $(shell curl-config --cflags)
LIB=gfxprim-curl
LIB_SRCS=gp_dialog_curl.c
LIB_HEADERS=$(wildcard *.h)
LIB_LDLIBS=$(shell curl-config --libs)

BIN=download_example
LDLIBS=$(shell gfxprim-config --libs-widgets --libs)
