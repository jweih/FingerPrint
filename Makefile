SRC_DIR := src
UTILS_DIR := utils
LIB_DIR := lib
INC_DIR	:= ./include

LOCAL_MODULE := PUNCH
PREFIX := arm-none-linux-gnueabi-


LOCAL_SRC_CFILES := \
		$(SRC_DIR)/main.c \
		$(UTILS_DIR)/debug.c \
		$(SRC_DIR)/lcd.c \
		$(SRC_DIR)/font.c \
		$(UTILS_DIR)/thread.c \
		$(SRC_DIR)/key.c \
		$(SRC_DIR)/wiegand.c \
		$(SRC_DIR)/rtc.c

LOCAL_C_INCLUDES += $(UTILS_DIR) $(INC_DIR)  

LOCAL_SHARED_LIBRARIES += freetype
LOCAL_LDFLAGS += -rdynamic	-pthread	
LOCAL_LDFLAGS += -L./$(LIB_DIR)   -finput-charset=GBK  -fexec-charset=UTF-8

include exec_rules.mk
