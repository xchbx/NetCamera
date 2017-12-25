DIRS=$(shell find . -maxdepth 3 -type d)
FILES=$(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
FILER=$(notdir $(FILES))
CUR_OBJS=$(patsubst %.c,%.o, $(FILER))
CUR_OBJS=$(foreach X,$(OBJS),$(OBJ_DIR)/$(X))
all:$(SUBDIRS) $(CUR_OBJS)

$(OBJ_DIR)/%.o: ./taskDemo/%.c
	@echo "Compiling $< ==> $@"
	@$(CC) -c $^ -o $(OBJ_DIR)/$@ $(LDFLAGS) $(INCLUDES) -L$(LIB)

ECHO:
	@echo $(FILES)

clean:
	rm -rf *.o
