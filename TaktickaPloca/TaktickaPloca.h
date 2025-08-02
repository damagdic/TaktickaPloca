#pragma once
#include <vector>
#include <windows.h>
#include "gdi.h"

class TaktickaPloca {
public:
    enum class ViewMode { FullField, HalfField };

    struct Player {
        float xRatio, yRatio;
        bool selected = false;
    };

    TaktickaPloca();

    void Resize(int w, int h);
    void SetViewMode(ViewMode mode);
    void Draw(HDC hdc);

    void OnMouseDown(int x, int y);
    bool OnMouseMove(int x, int y, WPARAM wParam);
    void OnMouseUp();

private:
    std::vector<Player> players;
    std::vector<Player> opponents;
    Player ball;

    int selectedIndex = -1;
    int selectedOpponentIndex = -1;
    int selectedBall = -1;

    int windowWidth = 0;
    int windowHeight = 0;
    ViewMode currentView = ViewMode::FullField;

    bool IsInsidePlayer(int x, int y, const Player& p);
    bool IsInsideBall(int x, int y);

    void DrawField(HDC dc, int width, int height);
    void DrawGoals(HDC dc, int width, int height);
    void DrawBoxes(HDC dc, int width, int height);
    void DrawPenaltyPointsAndCircle(HDC dc, int width, int height);
    void DrawPlayers(HDC dc, int width, int height);
    void DrawHalfField(HDC dc, int width, int height);
    void DrawPlayersHalf(HDC dc, int width, int height);
    void ResetPositions();
};