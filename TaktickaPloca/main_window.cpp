#include "main_window.h"
#include <windowsx.h>  // za GET_X_LPARAM i sl.
#include "TaktickaPloca.h"  
#include "Resource.h"


main_window::main_window() {
    // možeš inicijalizirati nešto ako želiš
}

LPCWSTR main_window::class_name() const {
    return L"MainWindowClass";
}

LRESULT main_window::on_message(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        ploca.SetWindowHandle(*this);  
        RECT r;
        GetClientRect(*this, &r);
        ploca.Resize(r.right, r.bottom);
        return 0;
    }
    case WM_SIZE: {
        ploca.Resize(LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(*this, nullptr, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        ploca.OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        InvalidateRect(*this, nullptr, FALSE);
        return 0;
    }
    case WM_MOUSEMOVE: {
        ploca.OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam);
            InvalidateRect(*this, nullptr, FALSE);
        return 0;
    }
    case WM_LBUTTONUP: {
        ploca.OnMouseUp();
        return 0;
    }
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_VIEW_FULL:
            ploca.SetViewMode(TaktickaPloca::ViewMode::FullField);
            InvalidateRect(*this, nullptr, TRUE);
            return 0;
        case ID_VIEW_HALF:
            ploca.SetViewMode(TaktickaPloca::ViewMode::HalfField);
            InvalidateRect(*this, nullptr, TRUE);
            return 0;
        case IDM_EXIT:
            PostMessage(*this, WM_CLOSE, 0, 0);
            return 0;
        case IDM_ABOUT: {
            MessageBox(*this, L"Author: Damir Magdić\nVerzija: 1.0", L"This is football tactial board! ENJOY", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
                      break;
        }
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(*this, &ps);
        ploca.Draw(hdc);
        EndPaint(*this, &ps);
        return 0;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(*this, message, wParam, lParam);
}