LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE	:= v8_base
LOCAL_SRC_FILES := ../support/android/libs/libv8_base.arm.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE	:= v8_nosnapshot
LOCAL_SRC_FILES :=  ../support/android/libs/libv8_nosnapshot.arm.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE    := jv8
LOCAL_SRC_FILES := jv8.cpp V8Runner.cpp $(wildcard $(LOCAL_PATH)/../support/v8/src/*.cpp)
LOCAL_C_INCLUDES := $(LOCAL_PATH)/../support/include
LOCAL_CFLAGS += -Wall -Wno-unused-function -Wno-unused-variable -O3 -funroll-loops -ftree-vectorize -ffast-math -fpermissive
LOCAL_STATIC_LIBRARIES := v8_base v8_nosnapshot
LOCAL_LDLIBS += -L$(SYSROOT)/usr/lib -llog
include $(BUILD_SHARED_LIBRARY)