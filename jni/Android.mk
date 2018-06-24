LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE    := dexload
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/elfGotHook/
LOCAL_C_INCLUDES +=$(LOCAL_PATH)/xhook/
# LOCAL_STATIC_LIBRARIES := static_openssl_crypto
# LOCAL_STATIC_LIBRARIES := static_openssl_ssl
LOCAL_SRC_FILES := packer.cpp  \
									 hook_instance.cpp \
									 byte_load.cpp \
									 utils.cpp
LOCAL_SRC_FILES += elfGotHook/elf_reader.cpp   \
									 elfGotHook/tools.cpp
LOCAL_SRC_FILES +=aes.c
# LOCAL_SRC_FILES +=  xhook/xhook.c \
#                     xhook/xh_core.c \
#                     xhook/xh_elf.c \
#                     xhook/xh_jni.c \
#                     xhook/xh_log.c \
#                     xhook/xh_util.c \
#                     xhook/xh_version.c


LOCAL_CFLAGS := -Wall
# LOCAL_CFLAGS +=-fpermissive
LOCAL_CFLAGS += -DNO_WINDOWS_BRAINDEATH #-Werror-pointer-arith  #-fvisibility=hidden
LOCAL_LDLIBS :=-llog -landroid
# LOCAL_LDLIBS +=$(LOCAL_PATH)/openssl/libcrypto.a
# LOCAL_LDLIBS +=$(LOCAL_PATH)/openssl/libssl.a
include $(BUILD_SHARED_LIBRARY)
