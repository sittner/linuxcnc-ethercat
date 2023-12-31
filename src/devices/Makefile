include ../../config.mk

include $(MODINC)

CFLAGS=$(EXTRA_CFLAGS)

srcs := $(wildcard *.c)
targets := $(subst .c,.o,$(srcs))

all: liblcecdevices.a

liblcecdevices.a: $(targets)
	$(Q)ar rcs liblcecdevices.a $(targets)

clean:
	rm -f *.o *.a
	rm -f *.sym *.tmp *.ver
