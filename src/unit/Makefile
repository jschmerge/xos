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
                    unit_program_config.o \
                    unit_timeutil.o \
                    unit_bithacks.o \
                    unit_average.o \
                    unit_bithacks.o \
                    unit_crc.o \
                    unit_codecvt_base.o \
                    unit_codecvt.o \
                    unit_unicode.o


unittest_LIBDEPS = filesystem codecvt environment


PROGRESS          ?= brief

ACTIONS      = check debug-check coverage-check

check_DEPS   = unittest
check_ACTION = if [ -n "$(TEST)" ]; then \
	  $$EXE -t $(TEST) -p $(PROGRESS); \
	else \
	  $$EXE -p $(PROGRESS); \
	fi

debug-check_DEPS = unittest.debug

debug-check_ACTION = $(check_ACTION)


coverage-check_DEPS = unittest.coverage

coverage-check_ACTION = $$EXE -p foo > /dev/null 2>&1 || true ; \
	$$EXE -o foo > /dev/null 2>&1 || true ; \
	$$EXE -t foo > /dev/null 2>&1 || true ; \
	$$EXE -h > /dev/null 2>&1 || true ; \
	$$EXE -r 1 -o none -l > /dev/null 2>&1 ; \
	if [ -n "$(TEST)" ]; then \
	  echo $$EXE -t $(TEST) -p verbose; \
	  $$EXE -t $(TEST) -p verbose; \
	else \
	  echo $$EXE -p verbose; \
	  $$EXE -p verbose; \
	fi

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
CFLAGS_debug      ?= $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug) \
                     -fsanitize=address
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
