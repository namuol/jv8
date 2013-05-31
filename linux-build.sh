#!/usr/bin/env bash
# NOTE: This script is VERY localized to my (louis.acresti@gmail.com) system.
# TODO: Design a proper build system. :] For now I'll just be distributing binaries as needed...
TARGET=ia32.debug

mkdir -p support/$TARGET/libs/.
rsync -tr support/v8/out/$TARGET/obj.target/tools/gyp/*.a support/$TARGET/libs/.
mkdir -p _dist/$TARGET

g++ \
  -Wl,-soname,libjv8.so \
  -shared \
  -I./support/v8/include/ \
  -I/usr/lib/jvm/java-6-openjdk/include/ \
  -I/usr/lib/jvm/java-6-openjdk/include/linux/ \
  -I./jni \
  ./jni/jv8.cpp \
  ./jni/V8Runner.cpp \
  ./support/$TARGET/libs/libv8_base.ia32.a \
  ./support/$TARGET/libs/libv8_nosnapshot.ia32.a \
  -o _dist/$TARGET/libjv8.so \
  -fomit-frame-pointer -ffast-math \
  -fdata-sections -ffunction-sections \
  -Wl,--gc-sections \
  -O3 \
  -Os -s
