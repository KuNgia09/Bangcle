LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE    := dexload
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/elfGotHook/

LOCAL_SRC_FILES := packer.cpp  hook_instance.cpp byte_load.cpp utils.cpp
LOCAL_SRC_FILES += elfGotHook\elf_reader.cpp   \
									elfGotHook\tools.cpp



LOCAL_CFLAGS := -Wall
# LOCAL_CFLAGS +=-fpermissive
LOCAL_CFLAGS += -DNO_WINDOWS_BRAINDEATH -Werror-pointer-arith
LOCAL_LDLIBS :=-llog -landroid
include $(BUILD_SHARED_LIBRARY)
