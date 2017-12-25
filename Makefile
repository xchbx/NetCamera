#@@@=====================================================================
CC=gcc
CFLAGS=
LDFLAGS=-lpthread -lrtmp
ROOT_DIR=$(shell pwd)/

LIB=$(ROOT_DIR)/middleware/librtmp/
SRC_DIR=$(ROOT_DIR)/project/
INCLUDES += -I $(SRC_DIR)/inc/ \
	-I $(ROOT_DIR)/middleware/librtmp/ \
	-I $(SRC_DIR)/inc/messageQueue \
	-I $(SRC_DIR)/inc/taskCommon \
	-I $(SRC_DIR)/inc/taskMain
SUBDIRS=middleware project

TARGET=fox
BUILD_DIR=$(ROOT_DIR)/build
OBJ_DIR=$(BUILD_DIR)/obj/

DIRS=$(shell find $(SRC_DIR) -maxdepth 3 -type d)
FILES=$(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
FILER=$(notdir $(FILES))
OBJS=$(patsubst %.c,%.o, $(FILER))
CUR_OBJS=$(foreach X,$(OBJS),$(OBJ_DIR)/$(X))

export CC LDFLAGS LIB BIN OBJ_DIR BIN_DIR ROOT_DIR INCLUDES
#@@@=====================================================================

all:cmd $(SUBDIRS) $(OBJ) ${TARGET}

cmd:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR)

test:
	@echo $(CUR_OBJS)
	@echo $(OBJS)
	@echo $(FILER)

${TARGET}: ${CUR_OBJS}
	@${CC} -o $(BUILD_DIR)/$@ $(CUR_OBJS) $(LDFLAGS) $(INCLUDES) -L$(LIB)
	@echo "Compile done."

$(SUBDIRS):ECHO
	make -C $@

ECHO:
	@echo $(SUBDIRS)

clean:
	@rm -rf $(BUILD_DIR)/*
	@for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done
