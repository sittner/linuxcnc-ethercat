include ../config.mk

EXTRA_CFLAGS := $(filter-out -Wframe-larger-than=%,$(EXTRA_CFLAGS))

.PHONY: all clean install

all: lcec_conf

clean:
	rm -f lcec_conf.o lcec_conf

install: lcec_conf
	mkdir -p $(DESTDIR)$(EMC2_HOME)/bin
	cp lcec_conf $(DESTDIR)$(EMC2_HOME)/bin/

lcec_conf: lcec_conf.o
	$(CC) -o $@ $< -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lexpat

%.o: %.c
	$(CC) -o $@ $(EXTRA_CFLAGS) -URTAPI -U__MODULE__ -DULAPI -Os -c $<

