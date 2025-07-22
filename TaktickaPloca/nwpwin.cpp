#include "nwpwin.h"

namespace nwp {

    HWND window::create(LPCWSTR className, LPCWSTR title, DWORD style,
        int x, int y, int width, int height,
        HWND parent, HMENU id) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = handle_msg_setup;
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpszClassName = className;
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wc.lpszMenuName = MAKEINTRESOURCE(128); 

        RegisterClass(&wc);

        return CreateWindow(className, title, style, x, y, width, height,
            parent, id, GetModuleHandle(nullptr), this);
    }


    HWND window::operator*() const {
        return hwnd;
    }

    LRESULT window::handle_message(UINT message, WPARAM wParam, LPARAM lParam) {
        return on_message(message, wParam, lParam);
    }

    LRESULT CALLBACK window::handle_msg_setup(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        if (message == WM_NCCREATE) {
            window* p = static_cast<window*>(((LPCREATESTRUCT)lParam)->lpCreateParams);
            p->hwnd = hwnd;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)p);
            SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)&handle_msg_thunk);
            return p->handle_message(message, wParam, lParam);
        }
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    LRESULT CALLBACK window::handle_msg_thunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
        window* p = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
        return p->handle_message(message, wParam, lParam);
    }

    application::application(HINSTANCE hInstance) : hInstance(hInstance) {}

    int application::run() {
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        return msg.wParam;
    }

}