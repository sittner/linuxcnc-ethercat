MODINC=$(shell comp --print-modinc)

all: rt user

realclean:
	rm -f rm *.ko *.mod.c *.o .*.cmd
	rm -f modules.order Module.symvers
	rm -rf .tmp_versions
	rm -f lcec_conf

obj-m += lcec.o
lcec-objs := \
    lcec_main.o \
    lcec_generic.o \
    lcec_el1xxx.o \
    lcec_el2521.o \
    lcec_el2xxx.o \
    lcec_el31x2.o \
    lcec_el40x1.o \
    lcec_el40x2.o \
    lcec_el41x2.o \
    lcec_el5101.o \
    lcec_el5151.o \
    lcec_el5152.o \
    lcec_el7342.o \
    lcec_el95xx.o \
    lcec_em7004.o \
    lcec_stmds5k.o \
    lcec_deasda.o \
    $(MATHSTUB)

include $(MODINC)

# dirty workaround to get the RTAI directory
RTAI_INCLUDE_DIR = $(subst /rtai.h,,$(firstword $(wildcard $(foreach i,$(subst -I,,$(filter -I%,$(RTFLAGS))), $(i)/rtai.h))))
RTAI_DIR = $(RTAI_INCLUDE_DIR)/..

user: lcec_conf

lcec_conf: lcec_conf.c
	gcc $(EXTRA_CFLAGS) -Wframe-larger-than=16384 -URTAPI -U__MODULE__ -DULAPI -Os -o $@ $< -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lexpat

install-user:
	mkdir -p $(DESTDIR)$(EMC2_HOME)/bin
	cp lcec_conf $(DESTDIR)$(EMC2_HOME)/bin

rt:
	$(MAKE) KBUILD_EXTRA_SYMBOLS="$(RTLIBDIR)/Module.symvers $(RTAI_DIR)/modules/ethercat/Module.symvers" -C $(KERNELDIR) SUBDIRS=`pwd` CC=$(CC) V=0 modules

install-rt:
	mkdir -p $(DESTDIR)$(RTLIBDIR)
	$(MAKE) install

