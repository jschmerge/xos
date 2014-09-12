####
# Project-specific details & settings
####
BUILD           ?= release

CXX = g++

INCLUDES_base    = 
INCLUDES_release = $(INCLUDES_base)
INCLUDES_debug   = $(INCLUDES_base)
INCLUDES         = $(INCLUDES_$(BUILD))

DEFINES_base     =
DEFINES_release  = $(DEFINES_base) -DNDEBUG
#DEFINES_release  = $(DEFINES_base)
DEFINES_debug    = $(DEFINES_base) -D_GLIBCXX_DEBUG --coverage
DEFINES          = $(DEFINES_$(BUILD))

CFLAGS_base      = -Wall -Wextra -pthread -march=native -mtune=native -g3
#CFLAGS_base      = -Wall -Wextra -pthread -march=native -mtune=native -O3 -g3 -flto
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

LIBS_base        = -lrt -ldl
LIBS_release     = $(LIBS_base)
LIBS_debug       = $(LIBS_base)
LIBS             = $(LIBS_$(BUILD))

TARGETS          = readtest filetypes

readtest_OBJS    = main.o descriptor.o
filetypes_OBJS   = filetypes.o 

include Makefile.include

