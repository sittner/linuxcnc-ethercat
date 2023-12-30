include ../config.mk
include Kbuild

include $(MODINC)

ifeq ($(BUILDSYS),kbuild)

# dirty workaround to get the RTAI directory
RTAIINCDIR = $(subst /rtai.h,,$(firstword $(wildcard $(foreach i,$(subst -I,,$(filter -I%,$(RTFLAGS))), $(i)/rtai.h))))
ifneq ($(RTAIINCDIR),)
  RTAIDIR = $(realpath $(RTAIINCDIR)/..)
endif

all:
	$(MAKE) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(RTLIBDIR)/Module.symvers $(RTAIDIR)/modules/ethercat/Module.symvers" -C $(KERNELDIR) SUBDIRS=`pwd` CC=$(CC) V=0 modules

else

LDFLAGS += -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lethercat

all: modules

# This is https://github.com/LinuxCNC/linuxcnc/pull/2610 patched in
# here manually, once we're able to depend on it being in upstream
# releases then we can remove this block entirely.
lcec.so:
	$(ECHO) Linking $@
	$(Q)ld -d -r -o $*.tmp $^
	$(Q)objcopy -j .rtapi_export -O binary $*.tmp $*.sym
	$(Q)(echo '{ global : '; tr -s '\0' < $*.sym | xargs -r0 printf '%s;\n' | grep .; echo 'local : * ; };') > $*.ver
	$(Q)$(CC) -shared -Bsymbolic $(LDFLAGS) -Wl,--version-script,$*.ver -o $@ $^ -lm
	$(Q)$(CC) -shared -Bsymbolic $(LDFLAGS) -Wl,--version-script,$*.ver -o $@ $^ -lm $(EXTRA_LDFLAGS)
	$(Q)chmod -x $@

endif

