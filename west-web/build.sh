#!/usr/bin/env bash
set -euo pipefail

# WEST Web Build Script
# Builds Rust → WASM, copies output to frontend, then builds the static site.

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
RUST_DIR="$SCRIPT_DIR/../src/west_rust"
FRONTEND_DIR="$SCRIPT_DIR/frontend"
WASM_DEST="$FRONTEND_DIR/src/wasm"

echo "=== WEST Web Build ==="

# 1. Build Rust to WASM
echo ""
echo "[1/3] Building Rust → WASM..."
cd "$RUST_DIR"
wasm-pack build --target web --release
echo "  ✓ WASM built ($(du -h pkg/west_rust_bg.wasm | cut -f1) .wasm)"

# 2. Copy WASM artifacts to frontend
echo ""
echo "[2/3] Copying WASM to frontend..."
mkdir -p "$WASM_DEST"
cp pkg/west_rust.js pkg/west_rust_bg.wasm pkg/west_rust.d.ts "$WASM_DEST/"
echo "  ✓ Copied to $WASM_DEST"

# 3. Build frontend
echo ""
echo "[3/3] Building frontend..."
cd "$FRONTEND_DIR"
npm install --silent
npm run build
echo "  ✓ Frontend built to $FRONTEND_DIR/dist/"

echo ""
echo "=== Build complete! ==="
echo "Static site is in: $FRONTEND_DIR/dist/"
echo ""
echo "To preview locally:  cd $FRONTEND_DIR && npx serve dist"
echo "To deploy: push dist/ contents to GitHub Pages"
