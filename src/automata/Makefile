TARGETS             = driver interval
LIB_TARGETS         = libautomata
libautomata_HEADERS = 
libautomata_OBJS    =

driver_OBJS         = driver.o $(libautomata_OBJS)
interval_OBJS       = interval_list.o $(libautomata_OBJS)

ifndef TOPDIR
  TOPDIR            = ..
  include $(TOPDIR)/Makefile.include
endif
