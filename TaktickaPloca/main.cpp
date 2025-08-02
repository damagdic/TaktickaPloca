#include "nwpwin.h"
#include "main_window.h"
#include "Resource.h"
#include "TaktickaPloca.h"
#include "gdi.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
    nwp::application app(hInstance);
    main_window wnd;
    wnd.create(wnd.class_name(), L"Tactical Board",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 600,
        nullptr, LoadMenu(app.get_instance(), MAKEINTRESOURCE(IDR_MAINMENU)));
    return app.run();
}