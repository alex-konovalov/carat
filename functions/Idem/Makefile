CC = gcc
CFLAGS = -g -fwritable-strings -DDIAG1
#TOPDIR = /usb/carat

INCL = $(TOPDIR)/include

AR = ar rvuc $(TOPDIR)/lib/functions.a

GLOBAL = -DTOPDIR=\"$(TOPDIR)\" -DTABLES=\"$(TOPDIR)/tables/\" \
         -DATOMS=\"$(TOPDIR)/tables/atoms/\" \
         -DTABLEDIM=\"$(TOPDIR)/tables/dim\"

COMP = $(CC) $(CFLAGS) -c $(GLOBAL) -I$(INCL)

OBJS = almost_decomposable_lattice.o\
       bravais_catalog.o\
       centr.o\
       min_pol.o\
       symbol.o\
       v4_catalog.o\
       z_equivalent.o

.c.o:
	$(COMP) $< -o $@

ALL:  $(OBJS)
	$(AR) *.o

clean:
	rm -f *.o

strip:
	strip *.o
