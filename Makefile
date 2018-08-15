#@@@=====================================================================
CC=mips-linux-uclibc-gnu-gcc
CXX=mips-linux-uclibc-gnu-g++
AR=mips-linux-uclibc-gnu-ar
LD=mips-linux-uclibc-gnu-ld

#CC=gcc
#AR=ar
#LD=ld

CFLAGS=
LDFLAGS=-lpthread
ROOT_DIR=$(shell pwd)/

LIB =-L$(ROOT_DIR)/middleware/librtmp/ \
	-L$(ROOT_DIR)/middleware/elog/ -lelog \
    -L$(ROOT_DIR)/middleware/t20/lib/ -limp -lsysutils -lalog \
    -L$(ROOT_DIR)/middleware/t20/lib/mxu/ -lmxu_merge -lmxu_objdetect -lmxu_contrib -lmxu_video -lmxu_imgproc -lmxu_core \
    -L$(ROOT_DIR)/middleware/t20/lib/iaac/ -lverify -liaac



SRC_DIR=$(ROOT_DIR)/project/
INCLUDES += -I $(SRC_DIR)/inc/ \
	-I $(ROOT_DIR)/middleware/librtmp/ \
	-I $(ROOT_DIR)/middleware/elog/ \
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

export CC AR LD LDFLAGS LIB BIN OBJ_DIR BIN_DIR ROOT_DIR INCLUDES
#@@@=====================================================================

all:cmd $(SUBDIRS) $(OBJ) ${TARGET}

cmd:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR)

test:
	@echo $(CUR_OBJS)
	@echo $(OBJS)
	@echo $(FILER)

${TARGET}: ${CUR_OBJS}
	@${CXX} $(LDFLAGS) -o $(BUILD_DIR)/$@ $^ $(INCLUDES) $(LIB)
	@echo "Compile done."

$(SUBDIRS):ECHO
	make -C $@

ECHO:
	@echo $(SUBDIRS)

clean:
	@rm -rf $(BUILD_DIR)/*
	@for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done
