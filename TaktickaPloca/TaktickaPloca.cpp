// TaktickaPloca.cpp : Defines the entry point for the application.
//
#include "Windows.h"
#include "framework.h"
#include "TaktickaPloca.h"
#include <vector>
#include <string>
#include <cmath>
#include <gdiplus.h>
#include <algorithm>
#pragma comment(lib, "Gdiplus.lib")

using namespace Gdiplus;

ULONG_PTR gdiplusToken;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
const int PLAYER_RADIUS = 20;
int windowWidth = 1, windowHeight = 1;

struct Player {
    float xRatio, yRatio; // od 0.0 do 1.0
    bool selected = false;
    int number = 0;
    std::string name;
};

std::vector<Player> players;
int selectedIndex = -1;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)

{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    GdiplusStartupInput gdiStartupInput;
    GdiplusStartup(&gdiplusToken, &gdiStartupInput, NULL);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_TAKTICKAPLOCA, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TAKTICKAPLOCA));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    GdiplusShutdown(gdiplusToken);

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TAKTICKAPLOCA));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_TAKTICKAPLOCA);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);

   RECT clientRect;
   GetClientRect(hWnd, &clientRect);
   windowWidth = clientRect.right - clientRect.left;
   windowHeight = clientRect.bottom - clientRect.top;

   UpdateWindow(hWnd);
   InvalidateRect(hWnd, NULL, TRUE);

   players = {
       {0.05f, 0.50f},
       {0.15f, 0.20f}, {0.15f, 0.40f}, {0.15f, 0.60f}, {0.15f, 0.80f},
       {0.30f, 0.20f}, {0.30f, 0.40f}, {0.30f, 0.60f}, {0.30f, 0.80f},
       {0.45f, 0.40f}, {0.45f, 0.60f}
   };

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//

bool IsInsidePlayer(int x, int y, const Player& p);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;


    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Dimenzije klijentskog prozora
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);
        int width = clientRect.right - clientRect.left;
        int height = clientRect.bottom - clientRect.top;

        // Ažuriraj globalne vrijednosti (važno!)
        windowWidth = width;
        windowHeight = height;

        // Dinamička veličina kružića
        int playerRadius = min(width, height) / 50;

        // 1. Pozadina (zeleno)
        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 128, 0));
        FillRect(hdc, &clientRect, greenBrush);
        DeleteObject(greenBrush);

        // 2. Bijele linije – teren
        HPEN whitePen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
        HPEN oldPen = (HPEN)SelectObject(hdc, whitePen);

        int marginX = width / 20;
        int marginY = height / 20;

        // Vanjski okvir
        MoveToEx(hdc, marginX, marginY, NULL);
        LineTo(hdc, width - marginX, marginY);
        LineTo(hdc, width - marginX, height - marginY);
        LineTo(hdc, marginX, height - marginY);
        LineTo(hdc, marginX, marginY);

        int goalWidth = (height - 2 * marginY) / 4;
        int goalOffsetY = (height - goalWidth) / 2;
        int goalDepth = width / 50;

        // Lijevi gol
        Rectangle(hdc,
            marginX - goalDepth, goalOffsetY,
            marginX, goalOffsetY + goalWidth);
        // Desni gol
        Rectangle(hdc,
            width - marginX, goalOffsetY,
            width - marginX + goalDepth, goalOffsetY + goalWidth);
        
        int boxWidth = width / 8;
        int boxHeight = goalWidth * 2;

        // šesnaesterci
        int leftBoxX1 = marginX;
        int leftBoxX2 = marginX + boxWidth;
        int boxY1 = (height - boxHeight) / 2;
        int boxY2 = (height + boxHeight) / 2;

        MoveToEx(hdc, leftBoxX1, boxY1, NULL);
        LineTo(hdc, leftBoxX2, boxY1);
        LineTo(hdc, leftBoxX2, boxY2);
        LineTo(hdc, leftBoxX1, boxY2);
        LineTo(hdc, leftBoxX1, boxY1);

        int rightBoxX1 = width - marginX - boxWidth;
        int rightBoxX2 = width - marginX;

        MoveToEx(hdc, rightBoxX1, boxY1, NULL);
        LineTo(hdc, rightBoxX2, boxY1);
        LineTo(hdc, rightBoxX2, boxY2);
        LineTo(hdc, rightBoxX1, boxY2);
        LineTo(hdc, rightBoxX1, boxY1);

         // peterci
        int smallBoxWidth = width / 20;
        int smallBoxHeight = goalWidth*1.2; 
        int smallBoxY1 = (height - smallBoxHeight) / 2;
        int smallBoxY2 = (height + smallBoxHeight) / 2;

        // Lijevi peterac
        MoveToEx(hdc, marginX, smallBoxY1, NULL);
        LineTo(hdc, marginX + smallBoxWidth, smallBoxY1);
        LineTo(hdc, marginX + smallBoxWidth, smallBoxY2);
        LineTo(hdc, marginX, smallBoxY2);
        LineTo(hdc, marginX, smallBoxY1);

        // Desni peterac
        MoveToEx(hdc, width - marginX, smallBoxY1, NULL);
        LineTo(hdc, width - marginX - smallBoxWidth, smallBoxY1);
        LineTo(hdc, width - marginX - smallBoxWidth, smallBoxY2);
        LineTo(hdc, width - marginX, smallBoxY2);
        LineTo(hdc, width - marginX, smallBoxY1);

        int penaltyRadius = 3; // mala točkica

        // Lijeva točka
        int penaltyLeftX = marginX + boxWidth * 0.75;
        int penaltyY = height / 2;
        Ellipse(hdc, penaltyLeftX - penaltyRadius, penaltyY - penaltyRadius,
            penaltyLeftX + penaltyRadius, penaltyY + penaltyRadius);

        // Desna točka
        int penaltyRightX = width - marginX - boxWidth * 0.75;
        Ellipse(hdc, penaltyRightX - penaltyRadius, penaltyY - penaltyRadius,
            penaltyRightX + penaltyRadius, penaltyY + penaltyRadius);

        // Linija sredine
        MoveToEx(hdc, width / 2, marginY, NULL);
        LineTo(hdc, width / 2, height - marginY);

        // Krug na centru
        int circleRadius = min(width, height) / 10;
        Ellipse(hdc,
            width / 2 - circleRadius,
            height / 2 - circleRadius,
            width / 2 + circleRadius,
            height / 2 + circleRadius
        );

        int arcRadius = circleRadius / 2;

        int leftArcCenterX = marginX + boxWidth; // x = linija šesnaesterca
        int arcCenterY = height / 2;             // sredina po visini

        Arc(hdc,
            leftArcCenterX - arcRadius, arcCenterY - arcRadius,
            leftArcCenterX + arcRadius, arcCenterY + arcRadius,
            leftArcCenterX, arcCenterY + arcRadius,
            leftArcCenterX, arcCenterY - arcRadius);

        // Desni šesnaesterac – polukrug
        int rightArcCenterX = width - marginX - boxWidth;

        Arc(hdc,
            rightArcCenterX - arcRadius, arcCenterY - arcRadius,
            rightArcCenterX + arcRadius, arcCenterY + arcRadius,
            rightArcCenterX, arcCenterY - arcRadius,
            rightArcCenterX, arcCenterY + arcRadius);

        // 3. Igrači
        HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
        HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, whiteBrush);

        for (const auto& p : players) {
            int px = static_cast<int>(marginX + p.xRatio * (width - 2 * marginX));
            int py = static_cast<int>(marginY + p.yRatio * (height - 2 * marginY));

            Ellipse(hdc, px - playerRadius, py - playerRadius,
                px + playerRadius, py + playerRadius);

            // Debug ispis koordinata
            char buf[64];
            sprintf_s(buf, "(%.2f, %.2f)", p.xRatio, p.yRatio);
            TextOutA(hdc, px + playerRadius + 2, py - playerRadius - 2, buf, (int)strlen(buf));
        }

        // Oslobađanje resursa
        SelectObject(hdc, oldPen);
        DeleteObject(whitePen);
        SelectObject(hdc, oldBrush);
        DeleteObject(whiteBrush);

        EndPaint(hWnd, &ps);
        break;
    }


    case WM_LBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        for (size_t i = 0; i < players.size(); ++i) {
            if (IsInsidePlayer(x, y, players[i])) {
                players[i].selected = true;
                selectedIndex = (int)i;
                break;
            }
        }
        break;
    }

    case WM_MOUSEMOVE:
    {
        if ((wParam & MK_LBUTTON) && selectedIndex != -1) {
            int marginX = windowWidth / 20;
            int marginY = windowHeight / 20;
            int fieldWidth = windowWidth - 2 * marginX;
            int fieldHeight = windowHeight - 2 * marginY;

            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);

            float xRatio = (mx - marginX) / static_cast<float>(fieldWidth);
            float yRatio = (my - marginY) / static_cast<float>(fieldHeight);

            // Ograniči unutar 0.0 – 1.0
            xRatio = std::clamp(xRatio, 0.0f, 1.0f);
            yRatio = std::clamp(yRatio, 0.0f, 1.0f);

            players[selectedIndex].xRatio = xRatio;
            players[selectedIndex].yRatio = yRatio;
            InvalidateRect(hWnd, NULL, TRUE);
        }
        break;

    case WM_LBUTTONUP:
    {
        if (selectedIndex != -1) {
            players[selectedIndex].selected = false;
            selectedIndex = -1;
        }
        break;
    }

    case WM_SIZE:
        windowWidth = LOWORD(lParam);
        windowHeight = HIWORD(lParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;


    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
    }
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

bool IsInsidePlayer(int mx, int my, const Player& p) {
	int marginX = windowWidth / 20;
	int marginY = windowHeight / 20;
	int fieldwidth = windowWidth - 2 * marginX;
	int fieldheight = windowHeight - 2 * marginY;

    int px = static_cast<int>(marginX + p.xRatio * fieldwidth);
    int py = static_cast<int>(marginY + p.yRatio * fieldheight);
    int radius = min(windowWidth, windowHeight) / 40;

    int dx = mx - px;
    int dy = my - py;
    return std::sqrt(dx * dx + dy * dy) <= radius;
}