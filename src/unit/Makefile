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
                    unit_average.o \
                    unit_bithacks.o \
                    unit_crc.o 

#                    unit_codecvt_utf8.o \
#                    unit_codecvt.o \
#                    unit_codecvt_base.o \

#                    unit_unicode.o \
#                    unit_codecvt_utf16.o \
#                    unit_codecvt_utf8_utf16.o \

unittest_LIBDEPS = filesystem codecvt environment


PROGRESS        ?= brief

ACTIONS      = check check-xml debug-check coverage-check

check_DEPS   = unittest
check-xml_DEPS   = unittest

check_ACTION = if [ -n "$(TEST)" ]; then \
	  $$EXE -t $(TEST) -p $(PROGRESS); \
	else \
	  $$EXE -p $(PROGRESS); \
	fi

check-xml_ACTION = if [ -n "$(TEST)" ]; then \
	  $$EXE -t $(TEST) -p none -o xml > unit-summary.xml; \
	else \
	  $$EXE -p none -o xml > unit-summary.xml; \
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

ifndef TOPDIR
  TOPDIR            = ..
  include $(TOPDIR)/Makefile.include
endif
