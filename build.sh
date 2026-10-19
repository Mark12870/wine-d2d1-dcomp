#!/bin/bash
# Builds giang17/wine into a relocatable runner. Run it inside org.gnome.Sdk//50 with the
# org.freedesktop.Sdk.Extension.llvm20 extension installed.
# usage: build.sh <wine source checkout> <output directory>
set -euo pipefail

source=$(realpath "$1")
output=$(realpath -m "$2")
export PATH=/usr/lib/sdk/llvm20/bin:$PATH

version=$(git -C "$source" describe --tags --match 'wine-*' HEAD)
name=wine-d2d1-dcomp-${version#wine-}
work=$(mktemp -d)
trap 'rm -rf "$work"' EXIT

mkdir -p "$output" "$work/build" "$work/stage"
cd "$work/build"
"$source/configure" \
    --prefix="/$name" \
    --enable-archs=i386,x86_64 \
    --disable-tests \
    --with-x \
    --with-wayland \
    --with-vulkan \
    --with-gstreamer
make -j"$(nproc)"
make install DESTDIR="$work/stage"
rm -rf "$work/stage/$name/include"

find "$work/stage/$name" -type f \( -name '*.so' -o -path "*/bin/*" \) -exec sh -c \
    'file -b "$1" | grep -q ELF && strip --strip-unneeded "$1"' sh {} \;

tar -C "$work/stage" -cf - "$name" | zstd -19 -T0 -o "$output/$name-x86_64.tar.zst"
git -C "$source" archive --format=tar --prefix="$name-source/" HEAD | zstd -19 -T0 -o "$output/$name-source.tar.zst"
(cd "$output" && sha256sum "$name-x86_64.tar.zst" "$name-source.tar.zst" > "$name.sha256")
echo "$name"
