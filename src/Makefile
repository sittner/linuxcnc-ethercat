all: all-deps realtime user
.PHONY: all all-deps install install-user install-realtime user realtime

-include ../config.mk
-include $(MODINC)
include Makefile.clean

RTLDFLAGS += -Wl,-rpath,$(LIBDIR)
RTEXTRA_LDFLAGS += -Wl,--whole-archive liblcecdevices.a -Wl,--no-whole-archive -L$(LIBDIR) -llinuxcnchal -lethercat -lrt

#EXTRA_CFLAGS += --std=c2x
EXTRA_CFLAGS += -Wall  # Increase debugging level

## targets
lcec-common-objs := lcec_devicelist.o lcec_ethercat.o lcec_pins.o
lcec-objs := lcec_main.o $(lcec-common-objs)
lcec-conf-srcs := $(wildcard lcec_conf*.c)
lcec-conf-objs = $(subst .c,.o,$(lcec-conf-srcs))
device-srcs := $(wildcard devices/*.c)
device-objs := $(subst .c,.o,$(device-srcs))
all-srcs := $(wildcard *.c devices/*.c)
all-deps := $(all-srcs:.c=.d)

## target-specific variables

# override EXTRA_CFLAGS for lcec_conf's .c files
$(lcec-conf-objs): EXTRA_CFLAGS := $(filter-out -Wframe-larger-than=%,$(EXTRA_CFLAGS))


## build rules

# Automatically generate .d dependency files.  Mostly from GNU Make's docs.
#
# GCC Flags used:
#
#   -MT foo: explicitly set the dependency target name to 'foo'.
#        Without this, the target for 'devices/lcec_generic.o' would just
#        be 'lcec_generic.o', which breaks the makefile.
#   -MM: generate dependencies, but ignore system .h files
#   -MG: assume that missing header files are generated and add them without
#        throwing an error.
#   -MP: add a dummy rule for each .h that will keep make from throwing
#        errors if headers are deleted without 'make clean'.
%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MT $(subst .d,.o,$@) -MM -MG -MP $(EXTRA_CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(all-srcs:.c=.d)
all-deps: $(all-deps)


liblcecdevices.a: $(device-objs)
	@$(ECHO) Creating library $@
	@$(Q)ar rcs liblcecdevices.a $(device-objs)


# Rules for building RTAI.  Currently disabled, and needs updated to
# work.  Ping @scottlaird if you need this and can't get it to work.
#
#ifeq ($(BUILDSYS),kbuild)
#
# dirty workaround to get the RTAI directory
#RTAIINCDIR = $(subst /rtai.h,,$(firstword $(wildcard $(foreach i,$(subst -I,,$(filter -I%,$(RTFLAGS))), $(i)/rtai.h))))
#ifneq ($(RTAIINCDIR),)
#  RTAIDIR = $(realpath $(RTAIINCDIR)/..)
#endif
#
#realtime:
#	$(MAKE) EXTRA_CFLAGS="$(EXTRA_CFLAGS)" KBUILD_EXTRA_SYMBOLS="$(RTLIBDIR)/Module.symvers $(RTAIDIR)/modules/ethercat/Module.symvers" -C $(KERNELDIR) SUBDIRS=`pwd` CC=$(CC) V=0 modules
#
#else

# This throws a warning about ignoring the old recipe for the target.
install: install-user install-realtime
	true  # override 'install' from $(MODINC)

realtime: lcec.so
user: lcec_conf

install-user: user
	mkdir -p $(DESTDIR)$(EMC2_HOME)/bin
	cp lcec_conf $(DESTDIR)$(EMC2_HOME)/bin/

install-realtime: realtime
	mkdir -p $(DESTDIR)$(RTLIBDIR)/
	cp lcec.so $(DESTDIR)$(RTLIBDIR)/

lcec.so: lcec_main.o $(lcec-common-objs) liblcecdevices.a
	$(ECHO) Linking $@
	$(Q)ld -d -r -o $*.tmp lcec_main.o $(lcec-common-objs)
	$(Q)objcopy -j .rtapi_export -O binary $*.tmp $*.sym
	$(Q)(echo '{ global : '; tr -s '\0' < $*.sym | xargs -r0 printf '%s;\n' | grep .; echo 'local : * ; };') > $*.ver
	$(Q)$(CC) -shared -Bsymbolic $(RTLDFLAGS) -Wl,--version-script,$*.ver -o $@ lcec_main.o $(lcec-comon-objs) -lm
	$(Q)$(CC) -shared -Bsymbolic $(RTLDFLAGS) -Wl,--version-script,$*.ver -o $@ lcec_main.o $(lcec-common-objs) -lm $(RTEXTRA_LDFLAGS)
	$(Q)chmod -x $@

lcec_conf: $(lcec-conf-objs) $(lcec-common-objs) liblcecdevices.a
	$(CC) -o $@ $(lcec-conf-objs) $(lcec-common-objs) -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lexpat -Wl,--whole-archive liblcecdevices.a -Wl,--no-whole-archive -lethercat -lm
