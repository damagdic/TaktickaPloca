#include "TaktickaPloca.h"
#include "gdi.h"
#include <windows.h>
#include <cmath>
#include <algorithm>
#include <string>
#include <fstream>


TaktickaPloca::TaktickaPloca() {
    InitPositions();
}

void TaktickaPloca::InitPositions() {
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
    lines.clear();         
    drawingLine = false;
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
            SetMode(Mode::Normal);
            players[i].selected = true;
            selectedIndex = i;
            return;
        }
    for (int i = 0; i < (int)opponents.size(); ++i)
        if (IsInsidePlayer(x, y, opponents[i])) {
            SetMode(Mode::Normal);
            opponents[i].selected = true;
            selectedOpponentIndex = i;
            return;
        }
    if (IsInsideBall(x, y)) {
        SetMode(Mode::Normal);
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
        DrawLines(memDC);
    }
    else {
        DrawHalfField(memDC, width, height);
        DrawPlayersHalf(memDC, width, height);
        DrawLines(memDC);
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

    // boxes
    int boxWidth = width / 8;
    int boxHeight = goalWidth * 2;
    int boxY1 = (height - boxHeight) / 2;
    int boxY2 = (height + boxHeight) / 2;

    Rectangle(dc, marginX, boxY1, marginX + boxWidth, boxY2);
    Rectangle(dc, width - marginX - boxWidth, boxY1, width - marginX, boxY2);

    // five meters line
    int smallBoxWidth = width / 20;
    int smallBoxHeight = static_cast<int>(goalWidth * 1.2);
    int smallBoxY1 = (height - smallBoxHeight) / 2;
    int smallBoxY2 = (height + smallBoxHeight) / 2;

    Rectangle(dc, marginX, smallBoxY1, marginX + smallBoxWidth, smallBoxY2);
    Rectangle(dc, width - marginX - smallBoxWidth, smallBoxY1, width - marginX, smallBoxY2);
}

void TaktickaPloca::DrawPenaltyPointsAndCircle(HDC dc, int width, int height) {


    int marginX = width / 20;
    int goalWidth = (height - 2 * (height / 20)) / 4;
    int boxWidth = width / 8;

    null_brush nb(dc);  

    // penalty spots
    int penaltyRadius = 3;
    int penaltyY = height / 2;
    int penaltyLeftX = static_cast<int>(marginX + boxWidth * 0.75);
    int penaltyRightX = static_cast<int>(width - marginX - boxWidth * 0.75);

    Ellipse(dc, penaltyLeftX - penaltyRadius, penaltyY - penaltyRadius,
        penaltyLeftX + penaltyRadius, penaltyY + penaltyRadius);
    Ellipse(dc, penaltyRightX - penaltyRadius, penaltyY - penaltyRadius,
        penaltyRightX + penaltyRadius, penaltyY + penaltyRadius);

    // center circle
    int circleRadius = (width < height ? width : height) / 10;
    Ellipse(dc, width / 2 - circleRadius, height / 2 - circleRadius,
        width / 2 + circleRadius, height / 2 + circleRadius);

    // halfcircles
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
    int playerRadius = (width < height ? width : height) / 40;

	// white players
    brush white(dc, RGB(255, 255, 255));
    selector selWhite(dc, white.get());

    SetBkMode(dc, TRANSPARENT);
    int pointSize = static_cast<int>(playerRadius * 1.2);  
    HFONT boldFont = create_bold_font(pointSize);
    font selFont(dc, boldFont);

    for (size_t i = 0; i < players.size(); ++i) {
        const auto& p = players[i];
        int px = static_cast<int>(marginX + p.xRatio * (width - 2 * marginX));
        int py = static_cast<int>(marginY + p.yRatio * (height - 2 * marginY));

        std::wstring number = std::to_wstring(i + 1);

        SetTextColor(dc, RGB(0, 0, 0));

        if (i == 0) {
            // blue keeper
            pen noPen(dc, RGB(0, 0, 0), 0, PS_NULL);
            selector selPen(dc, noPen.h);
            brush blue(dc, RGB(0, 128, 255));
            selector selBlue(dc, blue.get());
            ::Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
            RECT r{ px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius };
            DrawTextW(dc, number.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        else {
            
            pen noPen(dc, RGB(0, 0, 0), 0, PS_NULL);
            selector selPen(dc, noPen.h);
            ::Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
            RECT r{ px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius };
            DrawTextW(dc, number.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }


    }


    for (size_t i = 0; i < opponents.size(); ++i) {
        const auto& p = opponents[i];
        int px = static_cast<int>(marginX + p.xRatio * (width - 2 * marginX));
        int py = static_cast<int>(marginY + p.yRatio * (height - 2 * marginY));

        std::wstring number = std::to_wstring(i + 1);

        SetBkMode(dc, TRANSPARENT);
        font selFont(dc, boldFont);  
        SetTextColor(dc, RGB(255, 255, 255));

        if (i == 0) {
            // white keeper
            pen noPen(dc, RGB(0, 0, 0), 0, PS_NULL);
            selector selPen(dc, noPen.h);
            brush black(dc, RGB(0, 0, 0));
            selector selBlack(dc, black.get());
            Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
        }
        else {
            // red players
            pen noPen(dc, RGB(0, 0, 0), 0, PS_NULL);
            selector selPen(dc, noPen.h);
            brush redBrush(dc, RGB(255, 0, 0));
            selector selRed(dc, redBrush.get());
            Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
        }

        // number centered
        RECT r{ px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius };
        DrawTextW(dc, number.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    int ballRadius = playerRadius / 2;
    int bx = static_cast<int>(marginX + ball.xRatio * (width - 2 * marginX));
    int by = static_cast<int>(marginY + ball.yRatio * (height - 2 * marginY));

    DrawBall(dc, bx, by, ballRadius);
}

void TaktickaPloca::DrawHalfField(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;

    // window framework
    MoveToEx(dc, marginX, marginY, NULL);
    LineTo(dc, width - marginX, marginY);
    LineTo(dc, width - marginX, height - marginY);
    LineTo(dc, marginX, height - marginY);
    LineTo(dc, marginX, marginY);

    // goal
    int goalWidth = (height - 2 * marginY) / 4;
    int goalOffsetY = (height - goalWidth) / 2;
    int goalDepth = width / 50;

    Rectangle(dc, marginX - goalDepth, goalOffsetY, marginX, goalOffsetY + goalWidth);
}

void TaktickaPloca::DrawPlayersHalf(HDC dc, int width, int height) {
    int marginX = width / 20;
    int marginY = height / 20;
    int playerRadius = (width < height ? width : height) / 40;

    int fieldWidth = width - 2 * marginX;
    int fieldHeight = height - 2 * marginY;

    int pointSize = static_cast<int>(playerRadius * 1.2);
    HFONT boldFont = create_bold_font(pointSize);
    SetBkMode(dc, TRANSPARENT);

 
    for (size_t i = 0; i < players.size(); ++i) {
        const auto& p = players[i];

        int px = static_cast<int>(marginX + (p.xRatio * 2) * fieldWidth);  // scalling for half field
        int py = static_cast<int>(marginY + p.yRatio * fieldHeight);
        std::wstring number = std::to_wstring(i + 1);

        // selected font and color
        font selFont(dc, boldFont);
        SetTextColor(dc, RGB(0, 0, 0));

        pen noPen(dc, RGB(0, 0, 0), 0, PS_NULL);
        selector selPen(dc, noPen.h);

        if (i == 0) {
            // blue keeper
            brush blue(dc, RGB(0, 128, 255));
            selector selBlue(dc, blue.get());
            Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
        }
        else {
			// white players
            brush white(dc, RGB(255, 255, 255));
            selector selWhite(dc, white.get());
            Ellipse(dc, px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius);
        }

        // centered number
        RECT r{ px - playerRadius, py - playerRadius, px + playerRadius, py + playerRadius };
        DrawTextW(dc, number.c_str(), -1, &r, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    // ball
    int ballRadius = playerRadius / 2;
    int bx = static_cast<int>(marginX + ball.xRatio * fieldWidth * 2);
    int by = static_cast<int>(marginY + ball.yRatio * fieldHeight);

    DrawBall(dc, bx, by, ballRadius);
}

void TaktickaPloca::DrawBall(HDC dc, int cx, int cy, int r) {
    brush orange(dc, RGB(255, 140, 0)); selector selOrange(dc, orange.get());
    pen black(dc, RGB(0, 0, 0), 2); selector selBlack(dc, black.h);
    Ellipse(dc, cx - r, cy - r, cx + r, cy + r);

    brush blk(dc, RGB(0, 0, 0)); selector selBlk(dc, blk.get());
    Ellipse(dc, cx, cy, cx, cy);
}

void TaktickaPloca::DrawLines(HDC dc) {
    pen p(dc, RGB(0, 0, 0), 2);  // black line
    selector selPen(dc, p);

    int marginX = windowWidth / 20;
    int marginY = windowHeight / 20;
    int fieldWidth = windowWidth - 2 * marginX;
    int fieldHeight = windowHeight - 2 * marginY;
    float scaleX = (currentView == ViewMode::HalfField) ? 2.0f : 1.0f;

    for (const auto& line : lines) {
        int x1 = static_cast<int>(marginX + line.start.xRatio * fieldWidth * scaleX);
        int y1 = static_cast<int>(marginY + line.start.yRatio * fieldHeight);
        int x2 = static_cast<int>(marginX + line.end.xRatio * fieldWidth * scaleX);
        int y2 = static_cast<int>(marginY + line.end.yRatio * fieldHeight);

        // line
        MoveToEx(dc, x1, y1, nullptr);
        LineTo(dc, x2, y2);

        // arrow
        const double angle = atan2(y2 - y1, x2 - x1);
        const int arrowLength = 10;
        const int arrowWidth = 5;

        POINT arrow[3];
        arrow[0] = { x2, y2 };
        arrow[1] = {
            static_cast<int>(x2 - arrowLength * cos(angle - 0.3)),
            static_cast<int>(y2 - arrowLength * sin(angle - 0.3))
        };
        arrow[2] = {
            static_cast<int>(x2 - arrowLength * cos(angle + 0.3)),
            static_cast<int>(y2 - arrowLength * sin(angle + 0.3))
        };

        Polygon(dc, arrow, 3);
    }
}

void TaktickaPloca::ResetPositions() {
    InitPositions();
}

void TaktickaPloca::SetMode(Mode mode) {
    currentMode = mode;
    if (mode == Mode::Normal) {
        drawingLine = false;  
    }
}

bool TaktickaPloca::IsInAddLineMode() const {
    return currentMode == Mode::AddLine;
}

void TaktickaPloca::OnLineStart(int x, int y) {
    if (!IsInAddLineMode())
        return;

    lineStart = { x, y };
    drawingLine = true;
}

void TaktickaPloca::OnLineEnd(int x, int y) {
    if (currentMode != Mode::AddLine || !drawingLine)
        return;

    int marginX = windowWidth / 20;
    int marginY = windowHeight / 20;
    int fieldWidth = windowWidth - 2 * marginX;
    int fieldHeight = windowHeight - 2 * marginY;

    float scaleX = (currentView == ViewMode::HalfField) ? 2.0f : 1.0f;

    Line l;
    l.start.xRatio = ((lineStart.x - marginX) / (float)fieldWidth) / scaleX;
    l.start.yRatio = (lineStart.y - marginY) / (float)fieldHeight;

    l.end.xRatio = ((x - marginX) / (float)fieldWidth) / scaleX;
    l.end.yRatio = (y - marginY) / (float)fieldHeight;

    lines.push_back(l);

    drawingLine = false;
}

void TaktickaPloca::ClearLastLine() {
    if (!lines.empty())
        lines.pop_back();
    drawingLine = false;
}

void TaktickaPloca::ClearAllLines() {
    lines.clear();
    drawingLine = false;
}

bool TaktickaPloca::SaveTactic(const std::wstring& filename) const {
    std::wofstream file(filename);
    if (!file)
        return false;

    file << L"VIEWMODE " << (currentView == ViewMode::FullField ? L"FULL" : L"HALF") << L"\n";
    file << L"BALL " << ball.xRatio << L" " << ball.yRatio << L"\n";

    for (const auto& p : players)
        file << L"PLAYER " << p.xRatio << L" " << p.yRatio << L"\n";

    for (const auto& o : opponents)
        file << L"OPPONENT " << o.xRatio << L" " << o.yRatio << L"\n";

    for (const auto& line : lines)
        file << L"LINE " << line.start.xRatio << L" " << line.start.yRatio << L" "
        << line.end.xRatio << L" " << line.end.yRatio << L"\n";

    return true;
}

bool TaktickaPloca::LoadTactic(const std::wstring& filename) {
    std::wifstream file(filename);
    if (!file) return false;

    std::vector<Player> pTmp, oTmp;
    Player ballTmp = ball;
    std::vector<Line>  linesTmp = lines;
    ViewMode viewTmp = currentView;

    std::wstring type;
    while (file >> type) {
        if (type == L"VIEWMODE") {
            std::wstring mode; if (!(file >> mode)) return false;
            if (mode == L"FULL") viewTmp = ViewMode::FullField;
            else if (mode == L"HALF") viewTmp = ViewMode::HalfField;
            else return false;
        }
        else if (type == L"BALL") {
            if (!(file >> ballTmp.xRatio >> ballTmp.yRatio)) return false;
        }
        else if (type == L"PLAYER") {
            Player p; if (!(file >> p.xRatio >> p.yRatio)) return false;
            pTmp.push_back(p);
        }
        else if (type == L"OPPONENT") {
            Player o; if (!(file >> o.xRatio >> o.yRatio)) return false;
            oTmp.push_back(o);
        }
        else if (type == L"LINE") {
            Line l; if (!(file >> l.start.xRatio >> l.start.yRatio >> l.end.xRatio >> l.end.yRatio)) return false;
            linesTmp.push_back(l);
        }
        else {
            return false; 
        }
    }

    // everything is read successfully, update current state
    currentView = viewTmp;
    ball = ballTmp;
    players = pTmp.empty() ? players : pTmp;     
    opponents = oTmp.empty() ? opponents : oTmp;
    lines = linesTmp;

    return true;
}