####
# Project-specific details & settings
####
TARGETS            = 

LIB_TARGETS        = libcodecvt

libcodecvt_HEADERS = codecvt_mode.h \
                     codecvt_specializations.h \
                     codecvt_utf16.h \
                     codecvt_utf8.h \
                     codecvt_utf8_utf16.h \
                     utf_conversion_helpers.h

libcodecvt_OBJS    = codecvt_specializations.o \
                     codecvt_utf8.o \
                     codecvt_utf16.o \
                     codecvt_utf8_utf16.o

test_OBJS          = test.o $(libcodecvt_OBJS)

TOPDIR            ?= ..
include $(TOPDIR)/Makefile.include


INCLUDES_base     ?= -I yyy
INCLUDES_release  ?= $(INCLUDES_base)
INCLUDES_debug    ?= $(INCLUDES_base)
INCLUDES_coverage ?= $(INCLUDES_base)

DEFINES_base      ?=
DEFINES_release   ?= $(DEFINES_base) -DNDEBUG
DEFINES_debug     ?= $(DEFINES_base) -D_GLIBCXX_DEBUG
DEFINES_coverage  ?= $(DEFINES_base)

CFLAGS_base       ?= -Wall -Wextra -pthread -g3
CFLAGS_release    ?= $(CFLAGS_base) $(DEFINES_release) $(INCLUDES_release) -O3
CFLAGS_debug      ?= $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug)
CFLAGS_coverage   ?= $(CFLAGS_base) $(DEFINES_coverage) $(INCLUDES_coverage) \
                     -fprofile-arcs -ftest-coverage

CXXFLAGS_base     ?= -std=c++11
CXXFLAGS_release  ?= $(CXXFLAGS_base) $(CFLAGS_release)
CXXFLAGS_debug    ?= $(CXXFLAGS_base) $(CFLAGS_debug)
CXXFLAGS_coverage ?= $(CXXFLAGS_base) $(CFLAGS_coverage)

LDFLAGS_base      ?=
LDFLAGS_release   ?= $(LDFLAGS_base)
LDFLAGS_debug     ?= $(LDFLAGS_base) -rdynamic
LDFLAGS_coverage  ?= $(LDFLAGS_base)

LIBS_base         ?= -lrt -ldl
LIBS_release      ?= $(LIBS_base)
LIBS_debug        ?= $(LIBS_base)
LIBS_coverage     ?= $(LIBS_base)

