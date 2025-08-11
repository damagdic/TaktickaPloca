#pragma once
#include "nwpwin.h"
#include "TaktickaPloca.h"
#include "gdi.h"

class main_window : public nwp::window {
    TaktickaPloca ploca;

public:
    main_window();  // constructor
    LPCWSTR class_name() const override;
    LRESULT on_message(UINT message, WPARAM wParam, LPARAM lParam) override;
    void SaveTacticDialog();
    void LoadTacticDialog();

private:
    bool ctrlHeld_ = false;
    bool ctrlActivatedAddLine_ = false;
    std::wstring LoadResString(UINT id) const;
    void SyncViewRadio(UINT checkedID);
    void SyncNormalCheck(bool isNormal);

};
