CC = gcc
#TOPDIR = /usb/carat
CFLAGS = -fwritable-strings -DDIAG1 -g

INCL = $(TOPDIR)/include

AR = ar rvuc $(TOPDIR)/lib/functions.a

GLOBAL = -DTOPDIR=\"$(TOPDIR)\" -DTABLES=\"$(TOPDIR)/tables/\" \
         -DATOMS=\"$(TOPDIR)/tables/atoms/\" \
         -DTABLEDIM=\"$(TOPDIR)/tables/dim\"

COMP = $(CC) $(CFLAGS) -c $(GLOBAL) -I$(INCL)

OBJS = PoaN.o\
	name_fct.o\
	tsubgroups_fct.o\
	tsubgroups_fct_db.o\
	tsubgroups_tools.o\
	tSUPERgroups_fct_db.o

.c.o:
	$(COMP) $< -o $@

ALL:  $(OBJS)
	$(AR) *.o

clean:
	rm -f *.o

strip:
	strip *.o
