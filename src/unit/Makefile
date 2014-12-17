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
#TOPDIR            ?= ..
#include $(TOPDIR)/Makefile.include

#check: $(TARGETS)
#	@if [ -n "$(TEST)" ]; then \
#	  echo bin/unittest -t $(TEST) -p $(PROGRESS); \
#	  bin/unittest -t $(TEST) -p $(PROGRESS); \
#	else \
#	  echo bin/unittest -p $(PROGRESS); \
#	  bin/unittest -p $(PROGRESS); \
#	fi

#debug-check: $(DBG_TARGETS)
#	@if [ -n "$(TEST)" ]; then \
#	  echo bin/unittest.debug -t $(TEST) -p $(PROGRESS); \
#	  bin/unittest.debug -t $(TEST) -p $(PROGRESS); \
#	else \
#	  echo bin/unittest.debug -p $(PROGRESS); \
#	  bin/unittest.debug -p $(PROGRESS); \
#	fi


#coverage-check: $(COV_TARGETS)
#	@./bin/unittest.coverage -p foo > /dev/null 2>&1 || true
#	@./bin/unittest.coverage -o foo > /dev/null 2>&1 || true
#	@./bin/unittest.coverage -t foo > /dev/null 2>&1 || true
#	@./bin/unittest.coverage -h > /dev/null 2>&1 || true
#	@./bin/unittest.coverage -r 1 -o none -l > /dev/null 2>&1
#	@if [ -n "$(TEST)" ]; then \
#	  echo bin/unittest.coverage -t $(TEST) -p verbose; \
#	  bin/unittest.coverage -t $(TEST) -p verbose; \
#	else \
#	  echo bin/unittest.coverage -p verbose; \
#	  bin/unittest.coverage -p verbose; \
#	fi

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
