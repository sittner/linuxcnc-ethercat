all:
	$(MAKE) -C src all

clean:
	$(MAKE) -C src realclean

install:
	$(MAKE) -C src install-rt install-user
	mkdir -p $(DESTDIR)/usr/share/linuxcnc-ethercat
	cp -R examples $(DESTDIR)/usr/share/linuxcnc-ethercat

