VERSION=v1.0
prefix=
XLDFLAGS=
INC=

CC=gcc
LD=ld
AR=ar

LIBS_posix=

SO_posix=so.0
SO_EXT=$(SO_$(SYS))

SHARED=yes
SODEF_yes=-fPIC
SOLIB_yes=libelog.$(SO_EXT)
SO_DEF=$(SODEF_$(SHARED))
SO_LIB=$(SOLIB_$(SHARED))
SO_INST=$(SOINST_$(SHARED))

DEF=-DRTMPDUMP_VERSION=\"$(VERSION)\" $(CRYPTO_DEF) $(XDEF)
OPT=-O2
CFLAGS=-Wall $(XCFLAGS) $(INC) $(OPT) $(SO_DEF) -Wno-unused-but-set-variable

incdir=$(prefix)/include/librtmp
bindir=$(prefix)/bin
libdir=$(prefix)/lib
mandir=$(prefix)/man
BINDIR=$(DESTDIR)$(bindir)
INCDIR=$(DESTDIR)$(incdir)
LIBDIR=$(DESTDIR)$(libdir)
MANDIR=$(DESTDIR)$(mandir)

OBJS=rtmp.o log.o amf.o hashswf.o parseurl.o

all:	libelog.a

clean:
	rm -f *.o *.a *.so *.$(SO_EXT)

libelog.a: $(OBJS)
	$(AR) rs $@ $?

#librtmp.$(SO_EXT): $(OBJS)
#	$(CC) -shared -Wl,-soname,$@ $(LDFLAGS) -o $@ $^ $> $(CRYPTO_LIB)
#	ln -sf $@ librtmp.so

log.o: log.c log.h Makefile
rtmp.o: rtmp.c rtmp.h rtmp_sys.h handshake.h dh.h log.h amf.h Makefile
amf.o: amf.c amf.h bytes.h log.h Makefile
hashswf.o: hashswf.c http.h rtmp.h rtmp_sys.h Makefile
parseurl.o: parseurl.c rtmp.h rtmp_sys.h log.h Makefile

librtmp.pc: librtmp.pc.in Makefile
	sed -e "s;@prefix@;$(prefix);" -e "s;@VERSION@;$(VERSION);" \
		-e "s;@CRYPTO_REQ@;$(CRYPTO_REQ);" librtmp.pc.in > $@

install:	install_base $(SO_INST)

install_base:	librtmp.a librtmp.pc
	-mkdir -p $(INCDIR) $(LIBDIR)/pkgconfig $(MANDIR)/man3
	cp amf.h http.h log.h rtmp.h $(INCDIR)
	cp librtmp.a $(LIBDIR)
	cp librtmp.pc $(LIBDIR)/pkgconfig
	cp librtmp.3 $(MANDIR)/man3

install_so.0:	librtmp.so.0
	cp librtmp.so.0 $(LIBDIR)
	cd $(LIBDIR); ln -sf librtmp.so.0 librtmp.so

install_dll:	librtmp.dll
	cp librtmp.dll $(BINDIR)
