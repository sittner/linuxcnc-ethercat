all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src realclean

install: all
	$(MAKE) -C src install-rt install-user
	$(MAKE) -C examples install-examples

