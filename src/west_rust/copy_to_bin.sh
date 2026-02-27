#!/bin/bash
# Build the Rust binary in release mode and copy it to the C++ bin directory
set -e

CARGO_PROJECT_DIR="$(dirname "$0")"
BIN_DIR="$(realpath "$CARGO_PROJECT_DIR/../../bin")"

cd "$CARGO_PROJECT_DIR"
cargo build --release
cp target/release/west_rust "$BIN_DIR/west_rust"
echo "Copied release binary to $BIN_DIR/west_rust"