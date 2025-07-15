
#include <windows.h>
#include "TaktickaPloca.h"
#include <cmath>
#include <algorithm>

TaktickaPloca::TaktickaPloca(HWND hwnd) : hwnd(hwnd) {
    players = {
        {0.05f, 0.50f},
        {0.15f, 0.20f}, {0.15f, 0.40f}, {0.15f, 0.60f}, {0.15f, 0.80f},
        {0.30f, 0.20f}, {0.30f, 0.40f}, {0.30f, 0.60f}, {0.30f, 0.80f},
        {0.45f, 0.40f}, {0.45f, 0.60f}
    };

    opponents = {
    {0.95f, 0.50f},
    {0.85f, 0.20f}, {0.85f, 0.40f}, {0.85f, 0.60f}, {0.85f, 0.80f},
    {0.70f, 0.20f}, {0.70f, 0.40f}, {0.70f, 0.60f}, {0.70f, 0.80f},
    {0.55f, 0.40f}, {0.55f, 0.60f}
    };

    ball = { 0.5f, 0.5f };
}

void TaktickaPloca::Resize(int w, int h) {
    windowWidth = w;
    windowHeight = h;
}

bool TaktickaPloca::IsInsidePlayer(int mx, int my, const Player& p) {
    int marginX = windowWidth / 20;
    int marginY = windowHeight / 20;
    int fieldWidth = windowWidth - 2 * marginX;
    int fieldHeight = windowHeight - 2 * marginY;

    int px = static_cast<int>(marginX + p.xRatio * fieldWidth);
    int py = static_cast<int>(marginY + p.yRatio * fieldHeight);
    int radius = ((windowWidth < windowHeight) ? windowWidth : windowHeight) / 40;
    int dx = mx - px;
    int dy = my - py;
    return std::sqrt(dx * dx + dy * dy) <= radius;
}

bool TaktickaPloca::IsInsideBall(int mx, int my) {
    int marginX = windowWidth / 20;
    int marginY = windowHeight / 20;
    int fieldWidth = windowWidth - 2 * marginX;
    int fieldHeight = windowHeight - 2 * marginY;

    int bx = static_cast<int>(marginX + ball.xRatio * fieldWidth);
    int by = static_cast<int>(marginY + ball.yRatio * fieldHeight);
    int radius = ((windowWidth < windowHeight) ? windowWidth : windowHeight) / 80;

    int dx = mx - bx;
    int dy = my - by;
    return std::sqrt(dx * dx + dy * dy) <= radius;
}

void TaktickaPloca::OnMouseDown(int x, int y) {
    selectedIndex = -1;
    selectedOpponentIndex = -1;
    selectedBall = -1;

    for (size_t i = 0; i < players.size(); ++i) {
        if (IsInsidePlayer(x, y, players[i])) {
            players[i].selected = true;
            selectedIndex = static_cast<int>(i);
            return;
        }
    }

    for (size_t i = 0; i < opponents.size(); ++i) {
        if (IsInsidePlayer(x, y, opponents[i])) {
            opponents[i].selected = true;
            selectedOpponentIndex = static_cast<int>(i);
            return;
        }
    }
    if (IsInsideBall(x, y)) {
        ball.selected = true;
        selectedBall = 1;
        return;
    }
}
void TaktickaPloca::OnMouseMove(int x, int y, WPARAM wParam) {
        if (!(wParam & MK_LBUTTON)) return;

        int marginX = windowWidth / 20;
        int marginY = windowHeight / 20;
        int fieldWidth = windowWidth - 2 * marginX;
        int fieldHeight = windowHeight - 2 * marginY;

        float xRatio = (x - marginX) / static_cast<float>(fieldWidth);
        float yRatio = (y - marginY) / static_cast<float>(fieldHeight);

        xRatio = std::clamp(xRatio, 0.0f, 1.0f);
        yRatio = std::clamp(yRatio, 0.0f, 1.0f);

        if (selectedIndex != -1) {
            players[selectedIndex].xRatio = xRatio;
            players[selectedIndex].yRatio = yRatio;
        }
        else if (selectedOpponentIndex != -1) {
            opponents[selectedOpponentIndex].xRatio = xRatio;
            opponents[selectedOpponentIndex].yRatio = yRatio;
        }
        else if (selectedBall != -1) {
            ball.xRatio = xRatio;
            ball.yRatio = yRatio;
        }

        InvalidateRect(hwnd, NULL, FALSE);
    }

void TaktickaPloca::OnMouseUp() {
    if (selectedIndex != -1) {
        players[selectedIndex].selected = false;
        selectedIndex = -1;
    }
    if (selectedOpponentIndex != -1) {
        opponents[selectedOpponentIndex].selected = false;
        selectedOpponentIndex = -1;
    }
    if (selectedBall != -1) {
        ball.selected = false;
        selectedBall = -1;
    }
}

void TaktickaPloca::Draw(HDC hdc) {
    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HBITMAP oldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

    // Zeleni teren
    HBRUSH greenBrush = CreateSolidBrush(RGB(0, 128, 0));
    FillRect(memDC, &clientRect, greenBrush);
    DeleteObject(greenBrush);

    // Bijeli elementi
    HPEN whitePen = CreatePen(PS_SOLID, 3, RGB(255, 255, 255));
    HPEN oldPen = (HPEN)SelectObject(memDC, whitePen);

    DrawField(memDC, width, height);
    DrawGoals(memDC, width, height);
    DrawBoxes(memDC, width, height);
    DrawPenaltyPointsAndCircle(memDC, width, height);
    DrawPlayers(memDC, width, height);

    // Prijenos
    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    // Čišćenje
    SelectObject(memDC, oldPen);
    SelectObject(memDC, oldBitmap);
    DeleteObject(whitePen);
    DeleteObject(memBitmap);
    DeleteDC(memDC);
}

void TaktickaPloca::DrawField(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;

    MoveToEx(dc, marginX, marginY, NULL);
    LineTo(dc, width - marginX, marginY);
    LineTo(dc, width - marginX, height - marginY);
    LineTo(dc, marginX, height - marginY);
    LineTo(dc, marginX, marginY);

    MoveToEx(dc, width / 2, marginY, NULL);
    LineTo(dc, width / 2, height - marginY);
}

void TaktickaPloca::DrawGoals(HDC dc, int width, int height) {
	int marginX = width / 20;
    int marginY = height / 20;
	int goalWidth = (height - 2 * marginY) / 4;
	int goalOffsetY = (height - goalWidth) / 2;
	int goalDepth = width / 50;

	Rectangle(dc, marginX - goalDepth, goalOffsetY, marginX, goalOffsetY + goalWidth);
	Rectangle(dc, width - marginX, goalOffsetY, width - marginX + goalDepth, goalOffsetY + goalWidth);
}

void TaktickaPloca::DrawBoxes(HDC dc, int width, int height) {

    int marginX = width / 20;
    int marginY = height / 20;
    int goalWidth = (height - 2 * marginY) / 4;

    HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(dc, hNullBrush);

    // Šesnaesterci
    int boxWidth = width / 8;
    int boxHeight = goalWidth * 2;
    int boxY1 = (height - boxHeight) / 2;
    int boxY2 = (height + boxHeight) / 2;

    Rectangle(dc, marginX, boxY1, marginX + boxWidth, boxY2);
    Rectangle(dc, width - marginX - boxWidth, boxY1, width - marginX, boxY2);

    // Peterci
    int smallBoxWidth = width / 20;
    int smallBoxHeight = goalWidth * 1.2;
    int smallBoxY1 = (height - smallBoxHeight) / 2;
    int smallBoxY2 = (height + smallBoxHeight) / 2;

    Rectangle(dc, marginX, smallBoxY1, marginX + smallBoxWidth, smallBoxY2);
    Rectangle(dc, width - marginX - smallBoxWidth, smallBoxY1, width - marginX, smallBoxY2);

    SelectObject(dc, oldBrush);
}

void TaktickaPloca::DrawPenaltyPointsAndCircle(HDC dc, int width, int height) {
  
    int marginX = width / 20;
    int goalWidth = (height - 2 * (height / 20)) / 4;
    int boxWidth = width / 8;

    HBRUSH hNullBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    HBRUSH oldBrush = (HBRUSH)SelectObject(dc, hNullBrush);

    // Penal točke
    int penaltyRadius = 3;
    int penaltyY = height / 2;
    int penaltyLeftX = marginX + boxWidth * 0.75;
    int penaltyRightX = width - marginX - boxWidth * 0.75;

    Ellipse(dc, penaltyLeftX - penaltyRadius, penaltyY - penaltyRadius,
        penaltyLeftX + penaltyRadius, penaltyY + penaltyRadius);
    Ellipse(dc, penaltyRightX - penaltyRadius, penaltyY - penaltyRadius,
        penaltyRightX + penaltyRadius, penaltyY + penaltyRadius);

    // Krug centar
    int circleRadius = min(width, height) / 10;
    Ellipse(dc, width / 2 - circleRadius, height / 2 - circleRadius,
        width / 2 + circleRadius, height / 2 + circleRadius);

    // Polukrugovi
    int arcRadius = circleRadius / 2;
    int arcCenterY = height / 2;

    int leftArcCenterX = marginX + boxWidth;
    Arc(dc, leftArcCenterX - arcRadius, arcCenterY - arcRadius,
        leftArcCenterX + arcRadius, arcCenterY + arcRadius,
        leftArcCenterX, arcCenterY + arcRadius,
        leftArcCenterX, arcCenterY - arcRadius);

    int rightArcCenterX = width - marginX - boxWidth;
    Arc(dc, rightArcCenterX - arcRadius, arcCenterY - arcRadius,
        rightArcCenterX + arcRadius, arcCenterY + arcRadius,
        rightArcCenterX, arcCenterY - arcRadius,
        rightArcCenterX, arcCenterY + arcRadius);
	SelectObject(dc, oldBrush);
}

void TaktickaPloca::DrawPlayers(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;
    int playerRadius = min(width, height) / 50;

    HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
    HBRUSH oldBrush = (HBRUSH)SelectObject(dc, whiteBrush);
    for (const auto& p : players) {
        int px = static_cast<int>(marginX + p.xRatio * (width - 2 * marginX));
        int py = static_cast<int>(marginY + p.yRatio * (height - 2 * marginY));
        Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
    }
    DeleteObject(whiteBrush);

    HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
    SelectObject(dc, redBrush);
	for (const auto& p : opponents) {
		int px = static_cast<int>(marginX + p.xRatio * (width - 2 * marginX));
		int py = static_cast<int>(marginY + p.yRatio * (height - 2 * marginY));
		Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
	}
    SelectObject(dc, oldBrush);
    DeleteObject(redBrush);

    // Lopta
    int ballRadius = playerRadius / 2;
    int bx = static_cast<int>(marginX + ball.xRatio * (width - 2 * marginX));
    int by = static_cast<int>(marginY + ball.yRatio * (height - 2 * marginY));

    HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
    HBRUSH oldBallBrush = (HBRUSH)SelectObject(dc, blackBrush);
    Ellipse(dc, bx - ballRadius, by - ballRadius, bx + ballRadius, by + ballRadius);
    SelectObject(dc, oldBallBrush);
    DeleteObject(blackBrush);
}