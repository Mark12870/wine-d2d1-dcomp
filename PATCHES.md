# Wine patches

`build.sh` applies every file in `patches/` to the giang17/wine checkout before it configures, and
adds the files to the source archive it ships. A build that carries patches is named `-p<n>`, where
`n` counts the commits that changed `patches/`. Each section below says what would make the patch
unnecessary, and a probe in `probes/` tells when it has: the Patches workflow builds giang17/wine
without the patches, and `probes/check.sh` fails, naming the patch, once a probe finds the failure
gone.

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

iZotope Trash shows its preset list as a layered child over its whole editor, which is a WebView2
window above a GL canvas that Trash never draws into. Under the list, the pixels belong to the
WebView2 window, and only the X window holds them. Blending over them on every present compounds
the alpha, and the editor turns black within seconds. So where another child covers the GL parent
under a layered child, the patch keeps what that area showed before the layered child appeared and
blends over the copy. When the layered children go, it asks those children to repaint the area.

Verified on 11.0-887 in Cabinet: Vinyl's "Missing Authorization" panel draws inside the editor
(431 colours where the stock build gives 1) and a click on it changes the frame. Valhalla
Supermassive's GL editor, which has no layered child, captures the same with and without the patch.
In Bitwig, Trash's editor keeps its pixels around the open preset list for 30 s (13,000 colours
where the previous patch went black), a click picks a preset, and the editor is whole once the list
closes.

**When it can go.** When giang17's `d2d1-dcomp-11.0` or upstream Wine keeps a layered child inside
its parent. `probes/layered-child.c` draws one over an OpenGL parent, and `probes/x-parent.py` asks
the X server whether the child's window sits outside the parent's: today it does.

## crypt32-cert-only-pfx.patch

Upstream Wine [0b66fb09](https://gitlab.winehq.org/wine/wine/-/commit/0b66fb0932fb8b32e59f06740fbf2e9fd0590422)
and [986f9b66](https://gitlab.winehq.org/wine/wine/-/commit/986f9b66de3dffe26d339a607cbaf421baa5710a),
both in Wine 11.4, applied in that order.

**Failure.** `PFXImportCertStore` refuses a PKCS#12 blob that holds certificates but no private
key; Windows accepts one. Wine 11.0's `open_cert_store` requires
`gnutls_pkcs12_simple_parse` to return a key, and with none it fails ("The requested data were not
available").

iZotope's licences are such blobs. iZotope Product Portal fails to activate a product with
"Response Validation Failed", and a plug-in authorised elsewhere still loads as unauthorised, so
Vinyl shows only its "Trial Mode" card.

**Patch.** The first commit imports a certificate-only blob into the store. The second checks the
blob's MAC before it is parsed, as Windows does.

**When it can go.** When giang17's `d2d1-dcomp-11.0` is rebased onto Wine 11.4 or later.
`probes/pfx-cert-only.c` imports a certificate-only PKCS#12 that `check.sh` makes with OpenSSL:
today Wine refuses it.
