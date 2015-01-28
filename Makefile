MODINC=$(shell comp --print-modinc)

include $(MODINC)

all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src realclean

install:
	$(MAKE) -C src install-rt install-user
	mkdir -p $(DESTDIR)$(EMC2_HOME)/share/linuxcnc-ethercat
	cp -R examples $(DESTDIR)$(EMC2_HOME)/share/linuxcnc-ethercat

