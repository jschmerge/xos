export TOPDIR            = $(shell pwd)
export BUILD            ?= release

export INCLUDES_base     = -I $(TOPDIR)
export INCLUDES_release  = $(INCLUDES_base)
export INCLUDES_debug    = $(INCLUDES_base)
export INCLUDES_coverage = $(INCLUDES_base)
export INCLUDES          = $(INCLUDES_$(BUILD))

export DEFINES_base      =
export DEFINES_release   = $(DEFINES_base) -DNDEBUG
export DEFINES_debug     = $(DEFINES_base) -D_GLIBCXX_DEBUG
export DEFINES_coverage  = $(DEFINES_base)
export DEFINES           = $(DEFINES_$(BUILD))

export CFLAGS_base       = -Wall -Wextra -pthread -g3
export CFLAGS_release    = $(CFLAGS_base) $(DEFINES) $(INCLUDES) -O3
export CFLAGS_debug      = $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug)
export CFLAGS_coverage   = $(CFLAGS_base) $(DEFINES_coverage) \
                           $(INCLUDES_coverage) -fprofile-arcs -ftest-coverage
export CFLAGS            = $(CFLAGS_$(BUILD))

export CXXFLAGS_base     = -std=c++11
export CXXFLAGS_release  = $(CXXFLAGS_base) $(CFLAGS)
export CXXFLAGS_debug    = $(CXXFLAGS_base) $(CFLAGS_debug)
export CXXFLAGS_coverage = $(CXXFLAGS_base) $(CFLAGS_coverage)
export CXXFLAGS          = $(CXXFLAGS_$(BUILD))

export LDFLAGS_base      =
export LDFLAGS_release   = $(LDFLAGS_base)
export LDFLAGS_debug     = $(LDFLAGS_base) -rdynamic
export LDFLAGS_coverage  = $(LDFLAGS_base)
export LDFLAGS           = $(LDFLAGS_$(BUILD))

export USERLIBS          = -lfilesystem -lcodecvt
export USERLIBS_release  = $(USERLIBS)
export USERLIBS_debug    = $(USERLIBS:%=%.debug)
export USERLIBS_coverage = $(USERLIBS:%=%.coverage)

export LIBS_base         = -L $(shell pwd)/lib -lrt -ldl -lreadline \
                            $(USERLIBS_$(BUILD))
export LIBS_release      = $(LIBS_base)
export LIBS_debug        = $(LIBS_base)
export LIBS_coverage     = $(LIBS_base)
export LIBS              = $(LIBS_$(BUILD))

SUBDIRS           = codecvt filesystem unit

include Makefile.include

#check: all
#	$(MAKE) -C unit $@
#
#debug-check: debug
#	$(MAKE) -C unit $@
#
#coverage-check: coverage
#	$(MAKE) -C unit $@
#
#coverage.info: coverage
#	@./unit/bin/unittest.coverage -p foo > /dev/null 2>&1 || true
#	@./unit/bin/unittest.coverage -o foo > /dev/null 2>&1 || true
#	@./unit/bin/unittest.coverage -t foo > /dev/null 2>&1 || true
#	@./unit/bin/unittest.coverage -h > /dev/null 2>&1 || true
#	@./unit/bin/unittest.coverage -r 1 -o none -l > /dev/null 2>&1
#	./unit/bin/unittest.coverage -p verbose
#	lcov --no-external --capture --directory . --output-file $@ > /dev/null
#	#lcov --capture --directory . --output-file $@ > /dev/null
#
#covreport: coverage.info
#	rm -rf coverage-report
#	genhtml $< --demangle-cpp --legend --output-directory coverage-report
#	#genhtml $< --legend --output-directory coverage-report
#	xdg-open coverage-report/index.html 2> /dev/null &
#
#codecount:
#	@echo "HEADERS:"
#	@find . -name '*.h' | xargs wc -l
#	@echo "SOURCE:"
#	@find . -name '*.cc' | xargs wc -l
#
#clean:
#	$(MAKE) -C filesystem clean
#	$(MAKE) -C unit clean
#	$(MAKE) -C codecvt clean
#	rm -f *.info
#
#realclean:
#	$(MAKE) -C filesystem realclean
#	$(MAKE) -C unit realclean
#	$(MAKE) -C codecvt realclean
#	rm -f *.info
#	rm -rf coverage-report
#
