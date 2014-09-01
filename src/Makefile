all:
	$(MAKE) -C filesystem
	$(MAKE) -C unit

check: all
	$(MAKE) -C unit check

clean:
	$(MAKE) -C filesystem clean
	$(MAKE) -C unit clean

realclean:
	$(MAKE) -C filesystem realclean
	$(MAKE) -C unit realclean

