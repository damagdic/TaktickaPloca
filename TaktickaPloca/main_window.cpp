#include "main_window.h"
#include <windowsx.h>
#include "resource.h"
#include "gdi.h"
#include <commdlg.h>

main_window::main_window() {}

LPCWSTR main_window::class_name() const {
    return L"MainWindowClass";
}

LRESULT main_window::on_message(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
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
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (ploca.IsInAddLineMode())
            ploca.OnLineStart(x, y);
        else
            ploca.OnMouseDown(x, y);
        InvalidateRect(*this, nullptr, FALSE);
        return 0;
    }
    case WM_LBUTTONUP: {
        int x = GET_X_LPARAM(lParam);
        int y = GET_Y_LPARAM(lParam);
        if (ploca.IsInAddLineMode())
            ploca.OnLineEnd(x, y);
        else
            ploca.OnMouseUp();
        InvalidateRect(*this, nullptr, FALSE);
        return 0;
    }
    case WM_MOUSEMOVE: {
        if (ploca.OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), wParam))
            InvalidateRect(*this, nullptr, FALSE);
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
        case ID_EDIT_ADDLINE:
            ploca.SetMode(TaktickaPloca::Mode::AddLine);
            return 0;
        case ID_EDIT_NORMAL:
            ploca.SetMode(TaktickaPloca::Mode::Normal);
            return 0;
        case ID_FILE_SAVETACTIC:
            SaveTacticDialog();
            return 0;
        case ID_FILE_LOADTACTIC:
            LoadTacticDialog();
            return 0;
        case IDM_ABOUT:
            MessageBox(*this, L"Author: Damir Magdić\nVerzija: 1.0", L"This is football tactical board! ENJOY", MB_OK | MB_ICONINFORMATION);
            return 0;
        case IDM_EXIT:
            PostMessage(*this, WM_CLOSE, 0, 0);
            return 0;
        }
        break;
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

void main_window::SaveTacticDialog() {
    wchar_t filename[MAX_PATH] = L"";
    OPENFILENAME ofn = { sizeof(ofn) };
    ofn.hwndOwner = *this;
    ofn.lpstrFilter = L"Tactic Files (*.txt)\0*.txt\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_OVERWRITEPROMPT;
    ofn.lpstrDefExt = L"txt";

    if (GetSaveFileName(&ofn)) {
        if (!ploca.SaveTactic(filename))
            MessageBox(*this, L"Save failed", L"Error", MB_ICONERROR);
        else
            MessageBox(*this, L"Saved!", L"Success", MB_OK);
    }
}

void main_window::LoadTacticDialog() {
    wchar_t filename[MAX_PATH] = L"";
    OPENFILENAME ofn = { sizeof(ofn) };
    ofn.hwndOwner = *this;
    ofn.lpstrFilter = L"Tactic Files (*.txt)\0*.txt\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST;
    ofn.lpstrDefExt = L"txt";

    if (GetOpenFileName(&ofn)) {
        if (!ploca.LoadTactic(filename))
            MessageBox(*this, L"Load failed", L"Error", MB_ICONERROR);
        else {
            InvalidateRect(*this, nullptr, TRUE);
            MessageBox(*this, L"Loaded!", L"Success", MB_OK);
        }
    }
}
