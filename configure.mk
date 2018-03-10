ifeq (, $(COMP))
  COMP = $(shell which halcompile)
endif
ifeq (, $(COMP))
  COMP = $(shell which comp)
endif
ifeq (, $(COMP))
  $(error halcompile/comp executable not found or set)
endif

.PHONY: configure
configure:
	@echo "COMP = $(COMP)"
	@echo "MODINC = $(MODINC)"
	@echo "BUILDSYS = $(BUILDSYS)"
	@echo "KERNELDIR = $(KERNELDIR)"
	@echo "CC = $(CC)"
	@echo "RTAI = $(RTAI)"
	@echo "RTFLAGS = $(RTFLAGS)"
	@echo "KERNELRELEASE = $(KERNELRELEASE)"
	@echo "EXTRA_CFLAGS = $(EXTRA_CFLAGS)"
	@echo "USE_RTLIBM = $(USE_RTLIBM)"
	@echo "EMC2_HOME = $(EMC2_HOME)"
	@echo "RUN_IN_PLACE = $(RUN_IN_PLACE)"
	@echo "RTLIBDIR = $(RTLIBDIR)"
	@echo "LIBDIR = $(LIBDIR)"
	@echo "prefix = $(prefix)"

# include modinc
MODINC=$(shell $(COMP) --print-modinc)
ifeq (, $(MODINC))
  $(error Unable to get modinc path)
endif

include $(MODINC)

