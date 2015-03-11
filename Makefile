.PHONY: all configure install clean

all: configure
	@$(MAKE) -C src all

clean: configure
	@$(MAKE) -C src clean
	rm -f config.mk

install: configure
	@$(MAKE) -C src install
	@$(MAKE) -C examples install-examples

configure: config.mk

config.mk: configure.mk
	@$(MAKE) -s -f configure.mk > config.mk

