#!/usr/bin/env bash
set -e

SOURCE="./build/release/spacers"
DEST="/usr/bin/spacers"

make "$SOURCE"
sudo install -v -m 755 "$SOURCE" "$DEST"

echo "done"
