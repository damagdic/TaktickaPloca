#pragma once
#include <windows.h>
#include <string>

namespace nwp {

    class window {
    protected:
        HWND hwnd = nullptr;

    public:
        virtual ~window() = default;

        HWND create(LPCWSTR className, LPCWSTR title, DWORD style,
            int x, int y, int width, int height,
            HWND parent = 0, HMENU id = 0);

        virtual LPCWSTR class_name() const = 0;
        virtual LRESULT on_message(UINT message, WPARAM wParam, LPARAM lParam) = 0;

        HWND operator*() const;
        operator HWND() const {
            return hwnd;
        }

    protected:
        virtual LRESULT handle_message(UINT message, WPARAM wParam, LPARAM lParam);

        static LRESULT CALLBACK handle_msg_setup(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        static LRESULT CALLBACK handle_msg_thunk(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
    };

    class application {
        HINSTANCE hInstance;
    public:
        application(HINSTANCE hInstance);
        int run();
        HINSTANCE get_instance() const { return hInstance; }
    };

}
