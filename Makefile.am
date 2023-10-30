AUTOMAKE_OPTIONS = subdir-objects

bin_PROGRAMS = sane_scanner

sane_scanner_SOURCES = \
    main.c \
    src/http_handlers.c \
    src/scan_operations.c \
    src/utils.c

noinst_HEADERS = \
    include/base.h \
    include/http_handlers.h \
    include/scan_operations.h \
    include/utils.h

AM_CFLAGS = -Wall
LDADD = -lsane -lmicrohttpd
