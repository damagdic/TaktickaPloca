#pragma once
#include "nwpwin.h"
#include "TaktickaPloca.h"
#include "gdi.h"

class main_window : public nwp::window {
    TaktickaPloca ploca;

public:
    main_window();  // konstruktor
    LPCWSTR class_name() const override;
    LRESULT on_message(UINT message, WPARAM wParam, LPARAM lParam) override;
};