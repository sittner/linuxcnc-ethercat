all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src realclean

install:
	$(MAKE) -C src install-rt install-user

