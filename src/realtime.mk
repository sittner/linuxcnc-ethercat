include ../config.mk
include Kbuild

cc-option = $(shell if $(CC) $(CFLAGS) $(1) -S -o /dev/null -xc /dev/null \
             > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi ;)

.PHONY: all install

ifeq ($(BUILDSYS),kbuild)

module = $(patsubst %.o,%.ko,$(obj-m))

ifeq (,$(findstring -Wframe-larger-than=,$(EXTRA_CFLAGS)))
  EXTRA_CFLAGS += $(call cc-option,-Wframe-larger-than=2560)
endif

$(module):
	$(MAKE) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(RTLIBDIR)/Module.symvers $(RTAIDIR)/modules/ethercat/Module.symvers" -C $(KERNELDIR) SUBDIRS=`pwd` CC=$(CC) V=0 modules

else

module = $(patsubst %.o,%.so,$(obj-m))

EXTRA_CFLAGS := $(filter-out -Wframe-larger-than=%,$(EXTRA_CFLAGS))

$(module): $(lcec-objs)
	$(CC) -shared -o $@ $(lcec-objs) -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lethercat -lrt

%.o: %.c
	$(CC) -o $@ $(EXTRA_CFLAGS) -Os -c $<

endif

all: $(module)

install: $(module)
	mkdir -p $(DESTDIR)$(RTLIBDIR)
	cp $(module) $(DESTDIR)$(RTLIBDIR)/

