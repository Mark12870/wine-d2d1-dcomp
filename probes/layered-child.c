#include <windows.h>
#include <GL/gl.h>
#include <stdio.h>

static LRESULT CALLBACK handle(HWND window, UINT message, WPARAM wparam, LPARAM lparam)
{
    return DefWindowProcA(window, message, wparam, lparam);
}

int main(void)
{
    HINSTANCE instance = GetModuleHandleA(NULL);
    WNDCLASSA class = { CS_OWNDC, handle, 0, 0, instance, NULL, NULL, NULL, NULL, "probe" };
    RegisterClassA(&class);

    HWND parent = CreateWindowExA(0, "probe", "layered-child", WS_POPUP | WS_VISIBLE,
                                  0, 0, 200, 200, NULL, NULL, instance, NULL);
    HDC dc = GetDC(parent);
    PIXELFORMATDESCRIPTOR format = { sizeof(format), 1,
                                     PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                                     PFD_TYPE_RGBA, 24 };
    SetPixelFormat(dc, ChoosePixelFormat(dc, &format), &format);
    wglMakeCurrent(dc, wglCreateContext(dc));

    HWND child = CreateWindowExA(WS_EX_LAYERED, "probe", NULL, WS_CHILD | WS_VISIBLE,
                                 50, 50, 100, 100, parent, NULL, instance, NULL);
    BITMAPINFO info = { { sizeof(BITMAPINFOHEADER), 100, -100, 1, 32, BI_RGB } };
    DWORD *pixels;
    HDC memory = CreateCompatibleDC(NULL);
    SelectObject(memory, CreateDIBSection(memory, &info, DIB_RGB_COLORS, (void **)&pixels, NULL, 0));
    for (int i = 0; i < 100 * 100; i++) pixels[i] = 0xff00ff00;

    POINT origin = { 0, 0 };
    SIZE size = { 100, 100 };
    BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
    UpdateLayeredWindow(child, NULL, NULL, &size, memory, &origin, 0, &blend, ULW_ALPHA);

    for (DWORD end = GetTickCount() + 15000, told = 0; GetTickCount() < end;)
    {
        MSG message;
        while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) DispatchMessageA(&message);
        glClearColor(1, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        SwapBuffers(dc);
        Sleep(16);

        if (!told && GetTickCount() + 12000 > end)
        {
            printf("%lx %lx\n", (unsigned long)(ULONG_PTR)GetPropA(parent, "__wine_x11_whole_window"),
                   (unsigned long)(ULONG_PTR)GetPropA(child, "__wine_x11_whole_window"));
            fflush(stdout);
            told = 1;
        }
    }
    return 0;
}
