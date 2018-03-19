include ../config.mk
include Kbuild

include $(MODINC)

ifeq ($(BUILDSYS),kbuild)

all:
	$(MAKE) KBUILD_EXTRA_SYMBOLS="$(RTLIBDIR)/Module.symvers $(RTAIDIR)/modules/ethercat/Module.symvers" -C $(KERNELDIR) SUBDIRS=`pwd` CC=$(CC) V=0 modules

else

LDFLAGS += -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lethercat

all: modules

endif

