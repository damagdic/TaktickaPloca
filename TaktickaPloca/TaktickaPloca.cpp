#include "TaktickaPloca.h"
#include "gdi.h"
#include <windows.h>
#include <cmath>
#include <algorithm>

TaktickaPloca::TaktickaPloca() {
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

void TaktickaPloca::SetViewMode(ViewMode mode) {
    currentView = mode;
    ResetPositions();  
}

bool TaktickaPloca::IsInsidePlayer(int mx, int my, const Player& p) {
    int marginX = windowWidth / 20;
    int marginY = windowHeight / 20;
    int fieldWidth = windowWidth - 2 * marginX;
    int fieldHeight = windowHeight - 2 * marginY;
    float scaleX = (currentView == ViewMode::HalfField) ? 2.0f : 1.0f;
    int px = int(marginX + p.xRatio * fieldWidth * scaleX);
    int py = int(marginY + p.yRatio * fieldHeight);
    int radius = (windowWidth < windowHeight ? windowWidth : windowHeight) / 40;
    return std::hypot(mx - px, my - py) <= radius;
}

bool TaktickaPloca::IsInsideBall(int mx, int my) {
    int marginX = windowWidth / 20;
    int marginY = windowHeight / 20;
    int fieldWidth = windowWidth - 2 * marginX;
    int fieldHeight = windowHeight - 2 * marginY;
    float scaleX = (currentView == ViewMode::HalfField) ? 2.0f : 1.0f;
    int bx = int(marginX + ball.xRatio * fieldWidth * scaleX);
    int by = int(marginY + ball.yRatio * fieldHeight);
    int radius = (windowWidth < windowHeight ? windowWidth : windowHeight) / 80;
    return std::hypot(mx - bx, my - by) <= radius;
}

void TaktickaPloca::OnMouseDown(int x, int y) {
    selectedIndex = selectedOpponentIndex = selectedBall = -1;
    for (int i = 0; i < (int)players.size(); ++i)
        if (IsInsidePlayer(x, y, players[i])) {
            players[i].selected = true;
            selectedIndex = i;
            return;
        }
    for (int i = 0; i < (int)opponents.size(); ++i)
        if (IsInsidePlayer(x, y, opponents[i])) {
            opponents[i].selected = true;
            selectedOpponentIndex = i;
            return;
        }
    if (IsInsideBall(x, y)) {
        ball.selected = true;
        selectedBall = 1;
    }
}

bool TaktickaPloca::OnMouseMove(int x, int y, WPARAM wParam) {
    if (!(wParam & MK_LBUTTON))
        return false;

    int marginX = windowWidth / 20;
    int marginY = windowHeight / 20;
    int fieldWidth = windowWidth - 2 * marginX;
    int fieldHeight = windowHeight - 2 * marginY;

    float xRatio = (x - marginX) / (float)fieldWidth;
    float yRatio = (y - marginY) / (float)fieldHeight;

    if (currentView == ViewMode::HalfField)
        xRatio /= 2.0f;

    xRatio = std::clamp(xRatio, 0.0f, (currentView == ViewMode::HalfField) ? 0.5f : 1.0f);
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
    else return false;

    return true;
}

void TaktickaPloca::OnMouseUp() {
    if (selectedIndex != -1)
        players[selectedIndex].selected = false;
    if (selectedOpponentIndex != -1)
        opponents[selectedOpponentIndex].selected = false;
    if (selectedBall != -1)
        ball.selected = false;

    selectedIndex = selectedOpponentIndex = selectedBall = -1;
}

void TaktickaPloca::Draw(HDC hdc) {
    int width = windowWidth;
    int height = windowHeight;

    dc memDC(hdc);

    brush green(memDC, RGB(0, 128, 0));
    RECT r{ 0, 0, width, height };
    FillRect(memDC, &r, green.get());  


    pen white(memDC, RGB(255, 255, 255), 3);

    if (currentView == ViewMode::FullField) {
        DrawField(memDC, width, height);
        DrawGoals(memDC, width, height);
        DrawBoxes(memDC, width, height);
        DrawPenaltyPointsAndCircle(memDC, width, height);
        DrawPlayers(memDC, width, height);
    }
    else {
        DrawHalfField(memDC, width, height);
        DrawPlayersHalf(memDC, width, height);
    }
}


void TaktickaPloca::DrawField(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;

    pen p(dc, RGB(255, 255, 255), 2);

    MoveToEx(dc, marginX, marginY, nullptr);
    LineTo(dc, width - marginX, marginY);
    LineTo(dc, width - marginX, height - marginY);
    LineTo(dc, marginX, height - marginY);
    LineTo(dc, marginX, marginY);

    MoveToEx(dc, width / 2, marginY, nullptr);
    LineTo(dc, width / 2, height - marginY);
}

void TaktickaPloca::DrawGoals(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;
    int goalWidth = (height - 2 * marginY) / 4;
    int goalOffsetY = (height - goalWidth) / 2;
    int goalDepth = width / 50;

    pen p(dc, RGB(255, 255, 255), 2);

    Rectangle(dc, marginX - goalDepth, goalOffsetY, marginX, goalOffsetY + goalWidth);
    Rectangle(dc, width - marginX, goalOffsetY, width - marginX + goalDepth, goalOffsetY + goalWidth);
}

void TaktickaPloca::DrawBoxes(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;
    int goalWidth = (height - 2 * marginY) / 4;

    null_brush nb(dc);

    // Šesnaesterci
    int boxWidth = width / 8;
    int boxHeight = goalWidth * 2;
    int boxY1 = (height - boxHeight) / 2;
    int boxY2 = (height + boxHeight) / 2;

    Rectangle(dc, marginX, boxY1, marginX + boxWidth, boxY2);
    Rectangle(dc, width - marginX - boxWidth, boxY1, width - marginX, boxY2);

    // Peterci
    int smallBoxWidth = width / 20;
    int smallBoxHeight = static_cast<int>(goalWidth * 1.2);
    int smallBoxY1 = (height - smallBoxHeight) / 2;
    int smallBoxY2 = (height + smallBoxHeight) / 2;

    Rectangle(dc, marginX, smallBoxY1, marginX + smallBoxWidth, smallBoxY2);
    Rectangle(dc, width - marginX - smallBoxWidth, smallBoxY1, width - marginX, smallBoxY2);
}

void TaktickaPloca::DrawPenaltyPointsAndCircle(HDC dc, int width, int height) {

    auto min = [](int a, int b) { return (a < b) ? a : b; };

    int marginX = width / 20;
    int goalWidth = (height - 2 * (height / 20)) / 4;
    int boxWidth = width / 8;

    null_brush nb(dc);  

    // Penal točke
    int penaltyRadius = 3;
    int penaltyY = height / 2;
    int penaltyLeftX = static_cast<int>(marginX + boxWidth * 0.75);
    int penaltyRightX = static_cast<int>(width - marginX - boxWidth * 0.75);

    Ellipse(dc, penaltyLeftX - penaltyRadius, penaltyY - penaltyRadius,
        penaltyLeftX + penaltyRadius, penaltyY + penaltyRadius);
    Ellipse(dc, penaltyRightX - penaltyRadius, penaltyY - penaltyRadius,
        penaltyRightX + penaltyRadius, penaltyY + penaltyRadius);

    // Krug na centru
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
}

void TaktickaPloca::DrawPlayers(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;
    int playerRadius = (width < height ? width : height) / 50;

    // Igrači - bijeli
    brush white(dc, RGB(255, 255, 255));
    selector selWhite(dc, white.get());

    for (const auto& p : players) {
        int px = static_cast<int>(marginX + p.xRatio * (width - 2 * marginX));
        int py = static_cast<int>(marginY + p.yRatio * (height - 2 * marginY));
        ::Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
    }

    // Protivnici - crveni
    brush red(dc, RGB(255, 0, 0));
    selector selRed(dc, red.get());

    for (const auto& p : opponents) {
        int px = static_cast<int>(marginX + p.xRatio * (width - 2 * marginX));
        int py = static_cast<int>(marginY + p.yRatio * (height - 2 * marginY));
        ::Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
    }

    // Lopta - crna
    int ballRadius = playerRadius / 2;
    int bx = static_cast<int>(marginX + ball.xRatio * (width - 2 * marginX));
    int by = static_cast<int>(marginY + ball.yRatio * (height - 2 * marginY));

    brush black(dc, RGB(0, 0, 0));
    selector selBlack(dc, black.get());

    ::Ellipse(dc, bx - ballRadius, by - ballRadius, bx + ballRadius, by + ballRadius);
}

void TaktickaPloca::DrawHalfField(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;

    // Okvir preko cijelog prozora
    MoveToEx(dc, marginX, marginY, NULL);
    LineTo(dc, width - marginX, marginY);
    LineTo(dc, width - marginX, height - marginY);
    LineTo(dc, marginX, height - marginY);
    LineTo(dc, marginX, marginY);

    // Gol
    int goalWidth = (height - 2 * marginY) / 4;
    int goalOffsetY = (height - goalWidth) / 2;
    int goalDepth = width / 50;

    Rectangle(dc, marginX - goalDepth, goalOffsetY, marginX, goalOffsetY + goalWidth);
}

void TaktickaPloca::DrawPlayersHalf(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;
    int playerRadius = (width < height ? width : height) / 50;

    int fieldWidth = width - 2 * marginX;
    int fieldHeight = height - 2 * marginY;

    // Bijeli igrači
    brush white(dc, RGB(255, 255, 255));
    selector selWhite(dc, white.get());

    for (const auto& p : players) {
        int px = static_cast<int>(marginX + p.xRatio * fieldWidth * 2);
        int py = static_cast<int>(marginY + p.yRatio * fieldHeight);
        Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
    }

    // Lopta
    brush black(dc, RGB(0, 0, 0));
    selector selBlack(dc, black.get());

    int ballRadius = playerRadius / 2;
    int bx = static_cast<int>(marginX + ball.xRatio * fieldWidth * 2);
    int by = static_cast<int>(marginY + ball.yRatio * fieldHeight);

    Ellipse(dc, bx - ballRadius, by - ballRadius, bx + ballRadius, by + ballRadius);
}

void TaktickaPloca::ResetPositions() {
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