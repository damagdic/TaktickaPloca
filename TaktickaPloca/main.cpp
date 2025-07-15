#include <windows.h>
#include "resource.h"
#include "TaktickaPloca.h"

TaktickaPloca* ploca = nullptr;
HINSTANCE hInst;
WCHAR szWindowClass[] = L"TaktickaPlocaKlasa";
WCHAR szTitle[] = L"Taktička Ploča";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    hInst = hInstance;
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szWindowClass;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 1000, 600, nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    delete ploca;
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        ploca = new TaktickaPloca(hWnd);
        break;

    case WM_SIZE:
        if (ploca) {
            ploca->Resize(LOWORD(lParam), HIWORD(lParam));
            InvalidateRect(hWnd, NULL, FALSE);  
        }
        break;
    case WM_LBUTTONDOWN:
        if (ploca) ploca->OnMouseDown(LOWORD(lParam), HIWORD(lParam));
        break;

    case WM_MOUSEMOVE:
        if (ploca) ploca->OnMouseMove(LOWORD(lParam), HIWORD(lParam), wParam);
        break;

    case WM_LBUTTONUP:
        if (ploca) ploca->OnMouseUp();
        break;

    case WM_PAINT:
        if (ploca) {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            ploca->Draw(hdc);
            EndPaint(hWnd, &ps);
        }
        break;

    case WM_ERASEBKGND:
        return 1;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
