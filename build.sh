#!/usr/bin/env bash

NAME=JV8
V8_SRC_ROOT_DEFAULT=./v8
NUM_CPUS=1
PLATFORM_VERSION=android-8
ANT_TARGET=debug
V8_TARGET=android_arm.release
INSTALL=false
DEBUG=false

usage()
{
cat <<EOF
Usage: $0 options...

This script builds v8 against the Android NDK and a sample project skeleton that uses it.
Options:
  -h                  Show this help message and exit
  -s <v8_src>         The path to v8's project sourcetree's root. (default $V8_SRC_ROOT_DEFAULT)
  -v <v8_target>      Build target for v8. (default android_arm.release)
  -n <ndk_dir>        The path to the Android NDK. (default \$ANDROID_NDK_ROOT)
  -t <toolchain_dir>  The path to the Android's toolchain binaries. (default \$ANDROID_TOOLCHAIN)
  -p <platform>       The Android SDK version to support (default $V8_TARGET)
  -j <num-cpus>       The number of processors to use in building (default $NUM_CPUS)
  -i                  Install resulting example APK onto default device.
  -d                  Install resulting example APK and begin debugging via ndk-gdb. (implicitly sets -i)
EOF
}

red='\E[31;1m'
green='\E[32;3m'

function msg() {
    echo -n -e "$green"
    echo -n \-\>\  
    tput sgr0
    echo $@ >&1
}

function error() {
    echo -n -e "$red"
    echo -n \-\> ERROR:\  
    tput sgr0
    echo $@ >&2
}

function checkForErrors() {
    ret=$?
    if [ $ret -ne 0 ]
    then
        if [ -n "$1" ]
        then
            error "$@"
        else
            error "Unexpected error. Aborting."
        fi
        exit $ret
    fi
}

while getopts "hs:v:n:t:p:j:id" OPTION; do
  case $OPTION in
    h)
      usage
      exit
      ;;
    s)
      V8_SRC_ROOT=$OPTARG
      ;;
    v)
      V8_TARGET=$OPTARG
      ;;
    n)
      ANDROID_NDK_ROOT=$OPTARG
      ;;
    t)
      ANDROID_TOOLCHAIN=$OPTARG
      ;;
    j)
      NUM_CPUS=$OPTARG
      ;;
    p)
      PLATFORM_VERSION=$OPTARG
      ;;
    i)
      INSTALL=true
      ;;
    d)
      DEBUG=true
      ;;
    ?)
      usage
      exit
      ;;
  esac
done

if [[ -z "$V8_SRC_ROOT" ]]
then
  msg Please set \$V8_SRC_ROOT or use the -s option.
  usage
  exit
fi

if [[ -z "$ANDROID_NDK_ROOT" ]]
then
  msg Please set \$ANDROID_NDK_ROOT or use the -n option.
  usage
  exit
fi

if [[ -z "$ANDROID_TOOLCHAIN" ]]
then
  msg Please set \$ANDROID_TOOLCHAIN or use the -t option.
  usage
  exit
fi

# msg Updating/creating android project files...
# android update project --target $PLATFORM_VERSION --path .
# checkForErrors

msg Building v8 for android target...
pushd $V8_SRC_ROOT
make $V8_TARGET -j$NUM_CPUS
checkForErrors
popd

msg Copying static library files... 
mkdir -p support
rsync -tr $V8_SRC_ROOT/out/$V8_TARGET/obj.target/tools/gyp/*.a support/.
checkForErrors

msg Copying v8 header files...
mkdir -p support/include
rsync -tr $V8_SRC_ROOT/include/* support/include/.
checkForErrors

msg Building NDK libraries...
if $DEBUG
  then
  NDK_DEBUG=1
fi
$ANDROID_NDK_ROOT/ndk-build -j$NUM_CPUS
checkForErrors "Problem building JNI module."

# msg Building $ANT_TARGET APK...
# ant $ANT_TARGET
# checkForErrors "Problem building APK."

# if $INSTALL || $DEBUG
# then
#   if [[ $ANT_TARGET == "debug" ]]
#   then
#     APK_FILE=bin/$NAME-debug.apk
#   else
#     APK_FILE=bin/$NAME.apk
#   fi
#   msg Installing $APK_FILE to device...
#   adb install -r $APK_FILE
# fi

# if $DEBUG
# then
#   msg Starting ndk-gdb...

#   # HACK? We seem to need to wait, otherwise we get some sort of disconnection.
#   sleep 3
  
#   $ANDROID_NDK_ROOT/ndk-gdb --verbose --force --start
# fi
