all:
	$(MAKE) -C filesystem
	$(MAKE) -C unit

check: all
	$(MAKE) -C unit check

debug:
	$(MAKE) -C filesystem $@
	$(MAKE) -C unit $@

debugcheck:
	$(MAKE) -C filesystem debug
	$(MAKE) -C unit debug
	$(MAKE) -C unit debugcheck PROGRESS="dots"
	-./unit/bin/unittest.debug -p foo > /dev/null 2>&1
	-./unit/bin/unittest.debug -o foo > /dev/null 2>&1
	-./unit/bin/unittest.debug -t foo > /dev/null 2>&1
	-./unit/bin/unittest.debug -h > /dev/null 2>&1
	-./unit/bin/unittest.debug -r 1 -o none -l > /dev/null 2>&1
	$(MAKE) -C unit debugcheck PROGRESS=verbose
	$(MAKE) -C unit debugcheck PROGRESS=brief

coverage.info: debugcheck
	lcov --no-external --capture --directory . --output-file $@ > /dev/null

covreport: coverage.info
	rm -rf coverage-report
	genhtml $< --legend --output-directory coverage-report
	xdg-open coverage-report/index.html

clean:
	$(MAKE) -C filesystem clean
	$(MAKE) -C unit clean
	rm -f *.info
	rm -rf coverage-report

realclean:
	$(MAKE) -C filesystem realclean
	$(MAKE) -C unit realclean
	rm -f *.info
	rm -rf coverage-report

