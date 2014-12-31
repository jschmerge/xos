TARGETS             = 

LIB_TARGETS         = libcodecvt

libcodecvt_HEADERS  = codecvt_mode.h \
                      codecvt_specializations.h \
                      codecvt_utf16.h \
                      codecvt_utf8.h \
                      codecvt_utf8_utf16.h \
                      utf_conversion_helpers.h

libcodecvt_OBJS     = codecvt_specializations.o \
                      codecvt_utf8.o \
                      codecvt_utf16.o \
                      codecvt_utf8_utf16.o

test_OBJS           = test.o $(libcodecvt_OBJS)

ifndef TOPDIR
  TOPDIR            = ..
  include $(TOPDIR)/Makefile.include
endif
