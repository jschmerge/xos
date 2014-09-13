all:
	$(MAKE) -C filesystem
	$(MAKE) -C unit

check: all
	$(MAKE) -C unit check

debug:
	$(MAKE) -C filesystem $@
	$(MAKE) -C unit $@

coverage:
	$(MAKE) -C filesystem $@
	$(MAKE) -C unit $@

debugcheck:
	$(MAKE) -C unit debugcheck PROGRESS=verbose

coverage.info: coverage
	@./unit/bin/unittest.coverage -p foo > /dev/null 2>&1 || true
	@./unit/bin/unittest.coverage -o foo > /dev/null 2>&1 || true
	@./unit/bin/unittest.coverage -t foo > /dev/null 2>&1 || true
	@./unit/bin/unittest.coverage -h > /dev/null 2>&1 || true
	@./unit/bin/unittest.coverage -r 1 -o none -l > /dev/null 2>&1
	@./unit/bin/unittest.coverage -p brief
	./unit/bin/unittest.coverage -p verbose
	lcov --no-external --capture --directory . --output-file $@ > /dev/null
	#lcov --capture --directory . --output-file $@ > /dev/null

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

