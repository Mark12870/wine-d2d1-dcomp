# wine-d2d1-dcomp

Automatic builds of [giang17/wine](https://github.com/giang17/wine) for JUCE 8 plugin GUIs.

The builds are made for [Cabinet](https://github.com/Mark12870/cabinet), which runs them inside the
GNOME 50 runtime. This repository is not related to mklnln/wine-d2d1-dcomp.

Each build applies the patches in `patches/`, and a build that carries them is named `-p<n>`, where `n` counts the
commits that changed `patches/`. `PATCHES.md` says what each one fixes and when it can go. A push that changes
`patches/` or `upstream-commit` builds the giang17/wine commit named in `upstream-commit`, unless it is released.

Wine is licensed under the LGPL 2.1 or later, and so is this repository; see `LICENSE`.
