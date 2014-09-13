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
	-./unit/bin/unittest.debug -p dots \
	    -t Test_path_traits::test_encodable_types > /dev/null 2>&1
	-./unit/bin/unittest.debug -p foo > /dev/null 2>&1
	-./unit/bin/unittest.debug -o foo > /dev/null 2>&1
	-./unit/bin/unittest.debug -t foo > /dev/null 2>&1
	-./unit/bin/unittest.debug -h > /dev/null 2>&1
	-./unit/bin/unittest.debug -r 1 -o none -l > /dev/null 2>&1
#	$(MAKE) -C unit debugcheck PROGRESS=verbose
	-./unit/bin/unittest.debug -p brief \
	    -t Test_path_traits::test_encodable_types > /dev/null 2>&1
	-./unit/bin/unittest.debug -p verbose

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

