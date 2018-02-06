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
all:$(CUR_OBJS) libelog.a

$(CUR_OBJS):%.o:%.c
	@echo "Compiling $< ==> $@"
	@$(CC) -c $^ -o $@ $(INC)

libelog.a: $(CUR_OBJS)
	$(AR) rs $@ $?

clean:
	rm -f *.o *.a *.so *.$(SO_EXT)
