#!/bin/bash
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
BUILD_PATH="$SCRIPT_DIR/../build"

if [ -z "$1" ]; then
    echo "Usage ./build_current.sh <current_name>"
    exit 1
fi

cmake -G Ninja -S "$SCRIPT_DIR/../" -B $BUILD_PATH -DCMAKE_BUILD_TYPE=Release

# ----- For debugging memory issues:

# cmake -G Ninja -S "$SCRIPT_DIR/../" -B $BUILD_PATH \
# -DCMAKE_BUILD_TYPE=RelWithDebInfo \
# -DCMAKE_CXX_FLAGS="-fsanitize=address -fno-omit-frame-pointer" \
# -DCMAKE_EXE_LINKER_FLAGS="-fsanitize=address"

# ----- 

cmake --build $BUILD_PATH --target cli --config Release -j $(nproc)

cp "$SCRIPT_DIR/../build/src/cli/cli" "$SCRIPT_DIR/engines/$1"