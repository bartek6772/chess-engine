#!/bin/bash

SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)
TARGET_DIR="$SCRIPT_DIR/engines"
BUILD_PATH="$SCRIPT_DIR/../build"

if [ -z "$1" ]; then
    echo "Usage ./build.sh <git_tag>"
    exit 1
fi

if [[ -n $(git status --porcelain) ]]; then
    echo "--------------------------------------------------------"
    echo "ERROR: You have uncommitted changes or untracked files!"
    echo "Please commit or stash your work before running a test."
    echo "--------------------------------------------------------"
    git status -s
    exit 1
fi

TAG=$1
ENGINE_NAME="$TAG"

echo "Checking repository..."
git checkout "tags/$TAG" --detach || exit 1

echo "Building..."
rm -rf $BUILD_PATH
cmake -G Ninja -S "$SCRIPT_DIR/../" -B $BUILD_PATH -DCMAKE_BUILD_TYPE=Release
cmake --build $BUILD_PATH --target cli --config Release -j $(nproc)

echo "Moving binary..."
mkdir -p "$TARGET_DIR"
cp "$BUILD_PATH/src/cli/cli" "$TARGET_DIR/$ENGINE_NAME"

git checkout -
echo "Success"