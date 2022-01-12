#!/bin/sh

if [ -z "$FQBN" ]; then
    FQBN="esp32:esp32:featheresp32" # Fully Qualified Board Name
fi
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
        port=$2
        if [ -z "$port" ]; then
            port=/dev/ttyUSB0
        fi
        BUILD_FLAGS="$BUILD_FLAGS --upload --port $port"
        build
        ;;
    ci)
        CXX_FLAGS="$CXX_FLAGS -Werror -Wno-error=unused"
        build
        ;;
    *)
        cat << EOF
Usage:
    $0 compile         Compile only.
    $0 upload [port]   Compile and upload. Default port is /dev/ttyUSB0
    $0 ci              Compile and fail on any warning. Use this in CI.
EOF
        ;;
esac

