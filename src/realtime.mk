include ../config.mk
include Kbuild

ifeq ($(BUILDSYS),uspace)
LDFLAGS += -Wl,-rpath,$(LIBDIR) -L$(LIBDIR) -llinuxcnchal -lethercat
endif

include $(MODINC)

all: modules

