SUBDIRS=$(shell ls -l | grep ^d|grep -vE "inc" | awk '{print $$9}')
CUR_SOURCE=${wildcard *.c}
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
all:$(SUBDIRS) $(CUR_OBJS)
$(SUBDIRS):ECHO
	make -C $@
$(CUR_OBJS):%.o:%.c
	@echo "Compiling $< ==> $@"
	@$(CC) -c $^ -o $(OBJ_DIR)/$@ $(INCLUDES)
ECHO:
	@echo $(SUBDIRS)
	@echo $(INCLUDES)

clean:
	rm -rf *.o
