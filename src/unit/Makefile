####
# Project-specific details & settings
####
BUILD           ?= release

INCLUDES_base    =
INCLUDES_release = $(INCLUDES_base)
INCLUDES_debug   = $(INCLUDES_base)
INCLUDES         = $(INCLUDES_$(BUILD))

DEFINES_base     =
DEFINES_release  = $(DEFINES_base) -DNDEBUG
DEFINES_debug    = $(DEFINES_base) -D_GLIBCXX_DEBUG
DEFINES          = $(DEFINES_$(BUILD))

CFLAGS_base      = -Wall -Wextra -pthread -O3 -g3
CFLAGS_release   = $(CFLAGS_base) $(DEFINES) $(INCLUDES)
CFLAGS_debug     = $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug)
CFLAGS           = $(CFLAGS_$(BUILD))

CXXFLAGS_base    = -std=c++11
CXXFLAGS_release = $(CXXFLAGS_base) $(CFLAGS)
CXXFLAGS_debug   = $(CXXFLAGS_base) $(CFLAGS_debug)
CXXFLAGS         = $(CXXFLAGS_$(BUILD))

LDFLAGS_base     =
LDFLAGS_release  = $(LDFLAGS_base)
LDFLAGS_debug    = $(LDFLAGS_base) -rdynamic
LDFLAGS          = $(LDFLAGS_$(BUILD))

LIBS_base        = ../filesystem/lib/libfilesystem.a -lcppunit -lrt -ldl
LIBS_release     = $(LIBS_base)
LIBS_debug       = $(LIBS_base)
LIBS             = $(LIBS_$(BUILD))

TARGETS            = unittest

unittest_OBJS = cppunit-main.o \
                unit_path.o \
                unit_path_traits.o \
                unit_filesystem_error.o

include ../Makefile.include

check: $(TARGETS)
	bin/unittest -p brief
