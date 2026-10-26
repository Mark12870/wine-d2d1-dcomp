# Wine patches

`build.sh` applies every file in `patches/` to the giang17/wine checkout before it configures, and
adds the files to the source archive it ships. A build that carries patches is named `-p<n>`, where
`n` counts the commits that changed `patches/`. Each section below says what would make the patch
unnecessary.

## winex11-layered-child-compose.patch

No WineHQ bug covers layered child windows as of 2026-10. The nearest ones are about layered
top-level windows: [15118](https://bugs.winehq.org/show_bug.cgi?id=15118) and
[27470](https://bugs.winehq.org/show_bug.cgi?id=27470).

**Failure.** On Windows 8 and later, a child window can be `WS_EX_LAYERED` and paint itself with
`UpdateLayeredWindow`; the DWM blends it, alpha included, over whatever its parent presents.
winex11 gives such a child an X window of its own. That window is a separate top-level that the
window manager places anywhere, and the wineserver still clips the child's rectangle out of the
parent's visible region.

iZotope's plug-ins built on its "Glass" UI, Vinyl and Ozone Imager 2 among them, draw the editor as
one OpenGL canvas and show their consent and authorisation dialogs as such children. Under Wine
the editor stays black while a dialog is up, and the dialog is nowhere to be seen, so it cannot be
dismissed. An unauthorised or first-run plug-in never shows its UI.

**Patch.** In `dlls/winex11.drv`, a layered child painted through `UpdateLayeredWindow` gets no X
window. Its image stays in an offscreen surface, and each time the GL parent presents, it blends
its visible layered children on top, bottom-most first. When a child's image changes or it is shown
or hidden, the parent presents again. Children with `SetLayeredWindowAttributes`, and every other
window, take the existing paths.

Verified on 11.0-887 in Cabinet: Vinyl's "Missing Authorization" panel draws inside the editor
(431 colours where the stock build gives 1) and a click on it changes the frame. Valhalla
Supermassive's GL editor, which has no layered child, captures the same with and without the patch.

**When it can go.** When giang17's `d2d1-dcomp-11.0` or upstream Wine composites a layered child
into its parent. A build without the patch then shows an unauthorised Vinyl's "Missing
Authorization" panel inside the editor instead of a black frame.
