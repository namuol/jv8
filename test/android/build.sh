#!/usr/bin/env bash

NAME=JV8TestAndroid
NUM_CPUS=2
PLATFORM_VERSION=android-8
ANT_TARGET=debug
INSTALL=false
DEBUG=false
CLEAN=false

usage()
{
cat <<EOF
Usage: $0 options...

This script builds v8 against the Android NDK and a sample project skeleton that uses it.
Options:
  -h                  Show this help message and exit
  -t <toolchain_dir>  The path to the Android's toolchain binaries. (default \$ANDROID_TOOLCHAIN)
  -n <ndk_dir>        The path to the Android NDK. (default \$ANDROID_NDK_ROOT)
  -p <platform>       The Android SDK version to support (default $PLATFORM_VERSION)
  -j <num-cpus>       The number of processors to use in building (default $NUM_CPUS)
  -i                  Install resulting example APK onto default device.
  -d                  Install resulting example APK and begin debugging via ndk-gdb. (implicitly sets -i)
  -c                  Clean up everything.
  -z                  Dry run. Only print the commands that would be executed.
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

while getopts "hs:t:p:j:idzc" OPTION; do
  case $OPTION in
    h)
      usage
      exit
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
    n)
      ANDROID_NDK_ROOT=$OPTARG
      ;;
    d)
      DEBUG=true
      ;;
    z)
      DRY=echo
      ;;
    c)
      CLEAN=true
      ;;
    ?)
      usage
      exit
      ;;
  esac
done

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

if $CLEAN
then
  msg Cleaning up...
  $DRY ant clean
  checkForErrors "Problem running ant clean"
  exit
fi

if $INSTALL || $DEBUG
then
  INSTALL_ARG=install
fi

msg Building $ANT_TARGET APK...
$DRY ant $ANT_TARGET $INSTALL_ARG
checkForErrors "Problem building/installing APK."

if $DEBUG
then
  msg Starting ndk-gdb...

  # HACK? We seem to need to wait, otherwise we get some sort of disconnection.
  $DRY sleep 3
  
  $DRY $ANDROID_NDK_ROOT/ndk-gdb --verbose --force --start
fi
