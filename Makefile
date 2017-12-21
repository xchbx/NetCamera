#@@@=====================================================================
CC=gcc
CFLAGS=
LDFLAGS=-lpthread -lrtmp
ROOT_DIR=$(shell pwd)/

LIB=$(ROOT_DIR)/middleware/librtmp/
SRC_DIR=$(ROOT_DIR)/project
INCLUDES += -I $(SRC_DIR) \
	-I $(ROOT_DIR)/middleware/librtmp/
SUBDIRS=middleware project

TARGET=fox
BUILD_DIR=$(ROOT_DIR)/build
OBJ_DIR=$(BUILD_DIR)/obj

SRC=${wildcard ${SRC_DIR}/*.c}
OBJ=${patsubst %.c, $(OBJ_DIR)/%.o, ${notdir ${SRC}}}
export CC LDFLAGS LIB BIN OBJ_DIR BIN_DIR ROOT_DIR INCLUDES
#@@@=====================================================================

all:cmd $(SUBDIRS) $(OBJ) ${TARGET}

cmd:
	@mkdir -p $(BUILD_DIR) $(OBJ_DIR)	

test:
	@echo $(SRC)
	@echo $(OBJ)

${TARGET}: ${OBJ}
	@${CC} ${OBJ} -o $(BUILD_DIR)/$@ $(LDFLAGS) $(INCLUDES) -L$(LIB)
	@echo "Compile done."

$(SUBDIRS):ECHO
	make -C $@

ECHO:
	@echo $(SUBDIRS)

clean:
	@rm -rf $(BUILD_DIR)/*
