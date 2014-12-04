####
# Project-specific details & settings
####
TARGETS           = unittest

unittest_OBJS     = cppunit-main.o \
                    unit_file_status.o \
                    unit_directory_entry.o \
                    unit_fs_operations.o \
                    unit_filesystem_error.o \
                    unit_path_traits.o \
                    unit_directory_iterator.o \
                    unit_recursive_directory_iterator.o \
                    unit_path.o \
                    unit_path_iterator.o \
                    unit_bithacks.o \
                    unit-average.o \
                    unit-bithacks.o \
                    unit-crc.o \
                    unit_codecvt_base.o \
                    unit_codecvt.o \
                    unit_unicode.o


TOPDIR            ?= ..
include $(TOPDIR)/Makefile.include

PROGRESS ?= brief
bin/unittest:          ../filesystem/lib/libfilesystem.a \
                       ../codecvt/lib/libcodecvt.a

bin/unittest.debug:    ../filesystem/lib/libfilesystem.debug.a \
                       ../codecvt/lib/libcodecvt.debug.a

bin/unittest.coverage: ../filesystem/lib/libfilesystem.coverage.a \
                       ../codecvt/lib/libcodecvt.coverage.a

check: $(TARGETS)
	bin/unittest -p $(PROGRESS)

debug-check: $(DBG_TARGETS)
	bin/unittest.debug -p $(PROGRESS)

coverage-check: $(COV_TARGETS)
	bin/unittest.coverage -p $(PROGRESS)


INCLUDES_base     ?= -I $(TOPDIR)
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
LDFLAGS_release   ?= $(CFLAGG_release) $(LDFLAGS_base)
LDFLAGS_debug     ?= $(CFLAGG_debug) $(LDFLAGS_base) \
                     -rdynamic -fsanitize=address
LDFLAGS_coverage  ?= $(CFLAGG_coverage) $(LDFLAGS_base)

LIBS_base         += -lcppunit
LIBS_release      += $(LIBS_base)
LIBS_debug        += $(LIBS_base)
LIBS_coverage     += $(LIBS_base)


