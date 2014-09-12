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
	$(MAKE) -C unit debugcheck

clean:
	$(MAKE) -C filesystem clean
	$(MAKE) -C unit clean

realclean:
	$(MAKE) -C filesystem realclean
	$(MAKE) -C unit realclean

