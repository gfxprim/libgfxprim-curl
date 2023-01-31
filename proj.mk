CFLAGS+=$(shell gfxprim-config --cflags) $(shell curl-config --cflags)
LIB=gfxprim-curl
LIB_SRCS=gp_dialog_curl.c
LIB_HEADERS=$(wildcard *.h)

BIN=download_example
LDLIBS=$(shell gfxprim-config --libs --libs-widgets) $(shell curl-config --libs)
