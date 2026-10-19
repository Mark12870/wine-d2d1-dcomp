# wine-d2d1-dcomp

Automatic builds of [giang17/wine](https://github.com/giang17/wine) for JUCE 8 plugin GUIs.

Every day the `d2d1-dcomp-11.0` branch is checked, and a new commit becomes a release named after
`git describe`, for example `wine-11.0-776-g6b129c4832`. Each release carries:

- `wine-d2d1-dcomp-<version>-x86_64.tar.zst`: Wine, 64-bit with WoW64 (no 32-bit Unix libraries),
  relocatable, built in `org.gnome.Sdk//50` with LLVM 20 for the Windows side;
- `wine-d2d1-dcomp-<version>-source.tar.zst`: the exact source it was built from;
- `wine-d2d1-dcomp-<version>.sha256`: checksums of both.

The builds are made for [Cabinet](https://github.com/Mark12870/cabinet), which runs them inside the
GNOME 50 runtime. This repository is not related to mklnln/wine-d2d1-dcomp.

To build one by hand:

```sh
git clone --filter=blob:none --branch d2d1-dcomp-11.0 https://github.com/giang17/wine.git src
flatpak run --filesystem="$PWD" --env=TMPDIR="$PWD/tmp" --command=bash org.gnome.Sdk//50 ./build.sh src dist
```

Wine is licensed under the LGPL 2.1 or later, and so is this repository; see `LICENSE`.
