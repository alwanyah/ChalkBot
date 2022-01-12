#!/bin/sh

FQBN="esp32:esp32:featheresp32" # Fully Qualified Board Name
CXX_FLAGS="-Wall -Wextra $CXX_FLAGS"
BUILD_FLAGS="$BUILD_FLAGS" # '--warnings all' doesn't work for esp32 (their platform.txt ignores it)

build() {
    arduino-cli compile $BUILD_FLAGS --fqbn "$FQBN" --build-property "compiler.cpp.extra_flags=$CXX_FLAGS"
}

case $1 in
    compile)
        build
        ;;
    upload)
        BUILD_FLAGS="$BUILD_FLAGS --upload"
        build
        ;;
    ci)
        CXX_FLAGS="$CXX_FLAGS -Werror -Wno-error=unused"
        build
        ;;
    *)
        cat << EOF
Usage:
    $0 compile  Compile only.
    $0 upload   Compile and upload.
    $0 ci       Compile and fail on any warning. Use this in CI.
EOF
        ;;
esac

