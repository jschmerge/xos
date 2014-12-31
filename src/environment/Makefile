####
# Project-specific details & settings
####
TARGETS            = test

LIB_TARGETS        = libenvironment

libenvironment_HEADERS = program_config.h

libenvironment_OBJS    = program_config.o

test_OBJS          = test.o $(libenvironment_OBJS)

ifndef TOPDIR
  TOPDIR            = ..
  include $(TOPDIR)/Makefile.include
endif
