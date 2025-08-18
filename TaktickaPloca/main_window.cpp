#include "main_window.h"
#include <windowsx.h>
#include "resource.h"
#include "gdi.h"
#include <commdlg.h>
#include <tchar.h>

main_window::main_window() {}


void main_window::SyncViewRadio(UINT checkedID) {
    HMENU m = GetMenu(*this);
    HMENU view = GetSubMenu(m, 1); 
    CheckMenuRadioItem(view, ID_VIEW_FULL, ID_VIEW_HALF, checkedID, MF_BYCOMMAND);
}

void main_window::SyncNormalCheck(bool isNormal) {
    HMENU m = GetMenu(*this);
    HMENU edit = GetSubMenu(m, 2);  
    CheckMenuItem(edit, ID_EDIT_NORMAL,
        MF_BYCOMMAND | (isNormal ? MF_CHECKED : MF_UNCHECKED));
}

std::wstring main_window::LoadResString(UINT id) const {
    wchar_t buf[512]{};
    int n = LoadString(GetModuleHandle(nullptr), id, buf, (int)std::size(buf)); 
    return (n > 0) ? std::wstring(buf, n) : L"";
}


LPCWSTR main_window::class_name() const {
    return L"MainWindowClass";
}

LRESULT main_window::on_message(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        RECT r;
        GetClientRect(*this, &r);
        ploca.Resize(r.right, r.bottom);

        ploca.SetViewMode(TaktickaPloca::ViewMode::FullField);
        SyncViewRadio( ID_VIEW_FULL);
        ploca.SetMode(TaktickaPloca::Mode::Normal);
        SyncNormalCheck( true);
        return 0;
    }
    case WM_SIZE: {
        ploca.Resize(LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(*this, nullptr, FALSE);
        return 0;
    }
    case WM_LBUTTONDOWN: {
        SetCapture(*this);
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
        if (GetCapture() == *this)
            ReleaseCapture();
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

    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_CONTROL:

            if (!ctrlHeld_ && ((lParam & (1 << 30)) == 0)) {
                ctrlHeld_ = true;

                if (!ploca.IsInAddLineMode()) {
                    ploca.SetMode(TaktickaPloca::Mode::AddLine);
                    ctrlActivatedAddLine_ = true;      
                    SyncNormalCheck(false);     
                }
                else {
                    ctrlActivatedAddLine_ = false;     
                }
            }
            return 0;

        case VK_ESCAPE:

            ctrlHeld_ = false;
            ctrlActivatedAddLine_ = false;
            ploca.SetMode(TaktickaPloca::Mode::Normal);
            SyncNormalCheck(true);              
            InvalidateRect(*this, nullptr, FALSE);
            return 0;
        }
        break;
    }

    case WM_KEYUP: {
        switch (wParam) {
        case VK_CONTROL:
            ctrlHeld_ = false;

            if (ctrlActivatedAddLine_) {
                ploca.SetMode(TaktickaPloca::Mode::Normal);
                ctrlActivatedAddLine_ = false;
                SyncNormalCheck( true);        
                InvalidateRect(*this, nullptr, FALSE);
            }
            return 0;

        case VK_ESCAPE:

            ctrlHeld_ = false;
            ctrlActivatedAddLine_ = false;
            ploca.SetMode(TaktickaPloca::Mode::Normal);
            SyncNormalCheck(true);
            InvalidateRect(*this, nullptr, FALSE);
            return 0;
        }
        break;
    }

    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_VIEW_FULL:
            ploca.SetViewMode(TaktickaPloca::ViewMode::FullField);
            SyncViewRadio(ID_VIEW_FULL);
            InvalidateRect(*this, nullptr, TRUE);
            return 0;
        case ID_VIEW_HALF:
            ploca.SetViewMode(TaktickaPloca::ViewMode::HalfField);
			SyncViewRadio(ID_VIEW_HALF);
            InvalidateRect(*this, nullptr, TRUE);
            return 0;

        case ID_LINE_ADDLINE:
            ploca.SetMode(TaktickaPloca::Mode::AddLine);
            ctrlActivatedAddLine_ = false;
            SyncNormalCheck(false);
            return 0;

		case ID_LINE_CLEARLASTLINE:
			ploca.ClearLastLine();
			InvalidateRect(*this, nullptr, TRUE);
			return 0;
		case ID_LINE_CLEARALL:
			ploca.ClearAllLines();
			InvalidateRect(*this, nullptr, TRUE);
			return 0;

        case ID_EDIT_NORMAL:
            ploca.SetMode(TaktickaPloca::Mode::Normal);
            ctrlActivatedAddLine_ = false;          
            SyncNormalCheck(true);
            return 0;

        case ID_FILE_SAVETACTIC:
            SaveTacticDialog();
            return 0;
        case ID_FILE_LOADTACTIC:
            LoadTacticDialog();
            return 0;
        case IDM_ABOUT:
            MessageBox(*this, LoadResString(IDS_ABOUT_TEXT).c_str(), LoadResString(IDS_ABOUT_TITLE).c_str(), MB_OK | MB_ICONINFORMATION);
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
    TCHAR filename[MAX_PATH] = TEXT("");

    static const TCHAR* kFilter =
        TEXT("Tactic Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0"); 

    OPENFILENAME ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = *this;
    ofn.lpstrFilter = kFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = TEXT("txt");
    ofn.Flags = OFN_EXPLORER | OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

    if (GetSaveFileName(&ofn)) { 
        if (!ploca.SaveTactic(filename)) { 
            MessageBox(*this,
                LoadResString(IDS_SAVE_FAIL).c_str(),
                LoadResString(IDS_APP_TITLE).c_str(),
                MB_ICONERROR);
        }
        else {
            MessageBox(*this,
                LoadResString(IDS_SAVE_OK).c_str(),
                LoadResString(IDS_APP_TITLE).c_str(),
                MB_OK);
        }
    }
}

void main_window::LoadTacticDialog() {
    TCHAR filename[MAX_PATH] = TEXT("");

    static const TCHAR* kFilter =
        TEXT("Tactic Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0\0");

    OPENFILENAME ofn{};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = *this;
    ofn.lpstrFilter = kFilter;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrDefExt = TEXT("txt");
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    if (GetOpenFileName(&ofn)) { 
        if (!ploca.LoadTactic(filename)) {
            MessageBox(*this,
                LoadResString(IDS_LOAD_FAIL).c_str(),
                LoadResString(IDS_APP_TITLE).c_str(),
                MB_ICONERROR);
        }
        else {
            InvalidateRect(*this, nullptr, TRUE);
            MessageBox(*this,
                LoadResString(IDS_LOAD_OK).c_str(),
                LoadResString(IDS_APP_TITLE).c_str(),
                MB_OK);
        }
    }
}

