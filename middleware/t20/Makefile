VERSION=v1.0
prefix=
XLDFLAGS=
INC=

#CC=gcc
#LD=ld
#AR=ar


DEF=-DRTMPDUMP_VERSION=\"$(VERSION)\"
OPT=-O2
CFLAGS=-Wall $(XCFLAGS) $(INC) $(OPT) $(SO_DEF) -Wno-unused-but-set-variable


CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
all:$(CUR_OBJS)

$(CUR_OBJS):%.o:%.c
	@echo "Compiling $< ==> $@"

clean:
	@echo "Don't clean T20 lib"
