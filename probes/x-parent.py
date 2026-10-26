import ctypes
import sys

x11 = ctypes.CDLL("libX11.so.6")
x11.XOpenDisplay.restype = ctypes.c_void_p
x11.XQueryTree.argtypes = [ctypes.c_void_p, ctypes.c_ulong, ctypes.POINTER(ctypes.c_ulong),
                           ctypes.POINTER(ctypes.c_ulong), ctypes.POINTER(ctypes.c_void_p),
                           ctypes.POINTER(ctypes.c_uint)]

display = x11.XOpenDisplay(None)
parent, child = (int(window, 16) for window in sys.argv[1:3])

while child not in (0, parent):
    root, above, children, count = ctypes.c_ulong(), ctypes.c_ulong(), ctypes.c_void_p(), ctypes.c_uint()
    x11.XQueryTree(display, child, ctypes.byref(root), ctypes.byref(above), ctypes.byref(children),
                   ctypes.byref(count))
    if above.value == root.value:
        print("outside")
        sys.exit()
    child = above.value

print("inside")
