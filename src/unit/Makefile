####
# Project-specific details & settings
####
BUILD            ?= release

INCLUDES_base     =
INCLUDES_release  = $(INCLUDES_base)
INCLUDES_debug    = $(INCLUDES_base)
INCLUDES          = $(INCLUDES_$(BUILD))

DEFINES_base      =
DEFINES_release   = $(DEFINES_base) -DNDEBUG
DEFINES_debug     = $(DEFINES_base) -D_GLIBCXX_DEBUG
DEFINES           = $(DEFINES_$(BUILD))

CFLAGS_base       = -Wall -Wextra -pthread -O3 -g3
CFLAGS_release    = $(CFLAGS_base) $(DEFINES) $(INCLUDES)
CFLAGS_debug      = $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug)
CFLAGS_coverage   = $(CLFAGS_base) $(DEFINES_coverage) $(INCLUDES_coverage) \
                    -fprofile-arcs -ftest-coverage
CFLAGS            = $(CFLAGS_$(BUILD))

CXXFLAGS_base     = -std=c++11
CXXFLAGS_release  = $(CXXFLAGS_base) $(CFLAGS)
CXXFLAGS_debug    = $(CXXFLAGS_base) $(CFLAGS_debug)
CXXFLAGS_coverage = $(CXXFLAGS_base) $(CFLAGS_coverage)
CXXFLAGS          = $(CXXFLAGS_$(BUILD))

LDFLAGS_base      =
LDFLAGS_release   = $(LDFLAGS_base)
LDFLAGS_debug     = $(LDFLAGS_base) -rdynamic
LDFLAGS           = $(LDFLAGS_$(BUILD))

LIBS_base         = -lcppunit -lrt -ldl
# XXX FIXME - we need a way of tweaking this for the various build modes
LIBS_release      = ../filesystem/lib/libfilesystem.a $(LIBS_base)
LIBS_debug        = ../filesystem/lib/libfilesystem.debug.a $(LIBS_base)
LIBS_coverage     = ../filesystem/lib/libfilesystem.coverage.a $(LIBS_base)
LIBS              = $(LIBS_$(BUILD))

TARGETS           = unittest

unittest_OBJS     = cppunit-main.o \
                    unit_bithacks.o \
                    unit_average.o \
                    unit_unicode.o \
                    unit_filesystem_error.o \
                    unit_path_traits.o \
                    unit_path.o

PROGRESS ?= brief

include ../Makefile.include

check: $(TARGETS)
	bin/unittest -p $(PROGRESS)

debug-check: $(DBG_TARGETS)
	bin/unittest.debug -p $(PROGRESS)

coverage-check: $(DBG_TARGETS)
	bin/unittest.debug -p $(PROGRESS)
