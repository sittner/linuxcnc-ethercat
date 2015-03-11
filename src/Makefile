.PHONY: all install clean

all:
	@$(MAKE) -f user.mk all
	@$(MAKE) -f realtime.mk all

install:
	@$(MAKE) -f user.mk install
	@$(MAKE) -f realtime.mk install

clean:
	@$(MAKE) -f user.mk $@
	@$(MAKE) -f realtime.mk $@

