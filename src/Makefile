export TOPDIR            = $(shell pwd)

export INCLUDES_base     = -I $(TOPDIR) -I foobar
export INCLUDES_release  = $(INCLUDES_base)
export INCLUDES_debug    = $(INCLUDES_base)
export INCLUDES_coverage = $(INCLUDES_base)

export DEFINES_base      =
export DEFINES_release   = $(DEFINES_base) -DNDEBUG
export DEFINES_debug     = $(DEFINES_base) -D_GLIBCXX_DEBUG
export DEFINES_coverage  = $(DEFINES_base)

export CFLAGS_base       = -Wall -Wextra -pthread -g3
export CFLAGS_release    = $(CFLAGS_base) $(DEFINES) $(INCLUDES_release) -O3
export CFLAGS_debug      = $(CFLAGS_base) $(DEFINES_debug) $(INCLUDES_debug) -fsanitize=address
export CFLAGS_coverage   = $(CFLAGS_base) $(DEFINES_coverage) $(INCLUDES_coverage) -fprofile-arcs -ftest-coverage

export CXXFLAGS_base     = -std=c++11
export CXXFLAGS_release  = $(CXXFLAGS_base) $(CFLAGS_release)
export CXXFLAGS_debug    = $(CXXFLAGS_base) $(CFLAGS_debug)
export CXXFLAGS_coverage = $(CXXFLAGS_base) $(CFLAGS_coverage)

export LDFLAGS_base      =
export LDFLAGS_release   = $(LDFLAGS_base)
export LDFLAGS_debug     = $(LDFLAGS_base) -rdynamic -fsanitize=address
export LDFLAGS_coverage  = $(LDFLAGS_base)

export USERLIBS_base     = -lfilesystem -lcodecvt
export USERLIBS_release  = $(USERLIBS_base)
export USERLIBS_debug    = $(USERLIBS_base:%=%.debug)
export USERLIBS_coverage = $(USERLIBS_base:%=%.coverage)

export LIBS_base         = -L $(shell pwd)/lib -lrt -ldl
export LIBS_release      = $(LIBS_base) $(USERLIBS_release)
export LIBS_debug        = $(LIBS_base) $(USERLIBS_debug)
export LIBS_coverage     = $(LIBS_base) $(USERLIBS_coverage)

SUBDIRS                  = codecvt filesystem environment unit

include Makefile.include

# check debug-check converage-check: all
# 	$(MAKE) -C unit $@

# coverage.info:
# 	$(MAKE) coverage
# 	@./unit/bin/unittest.coverage -p foo > /dev/null 2>&1 || true
# 	@./unit/bin/unittest.coverage -o foo > /dev/null 2>&1 || true
# 	@./unit/bin/unittest.coverage -t foo > /dev/null 2>&1 || true
# 	@./unit/bin/unittest.coverage -h > /dev/null 2>&1 || true
# 	@./unit/bin/unittest.coverage -r 1 -o none -l > /dev/null 2>&1
# 	./unit/bin/unittest.coverage -p verbose
# 	lcov --no-external --capture --directory . --output-file $@ > /dev/null

# covreport: coverage.info
# 	rm -rf coverage-report
# 	genhtml $< --demangle-cpp --legend --output-directory coverage-report
# 	xdg-open coverage-report/index.html 2> /dev/null &

# codecount:
# 	@LC_ALL=en_GB.utf8 \
# 		/usr/bin/printf "     Headers: %' 7d\n" \
# 		$$(find . -name '*.h' | \
# 		   xargs wc -l | tail -1 | sed -e 's/total//g')
# 	@LC_ALL=en_GB.utf8 \
# 		/usr/bin/printf "      Source: %' 7d\n" \
# 		$$(find . -name '*.cc' -o -name '*.c' | \
# 		   xargs wc -l | tail -1 | sed -e 's/total//g')
# 	@LC_ALL=en_GB.utf8 \
# 		/usr/bin/printf "Total Source: %' 7d\n" \
# 		$$(find . -name '*.cc' -o -name '*.c' -o -name '*.h' | \
# 		   xargs wc -l | tail -1 | sed -e 's/total//g')
# 	@LC_ALL=en_GB.utf8 \
# 		/usr/bin/printf "       Build: %' 7d\n" \
# 		$$(find . -name 'Makefile*' \
# 		   | xargs wc -l | tail -1 | sed -e 's/total//g')
