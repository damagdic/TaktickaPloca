#pragma once
#include <windows.h>
#include <vector>
#include <string>

class TaktickaPloca {
public:
    TaktickaPloca(HWND hwnd);
    void Draw(HDC hdc);
    void DrawField(HDC dc, int width, int height);
    void DrawGoals(HDC dc, int width, int height);
    void DrawBoxes(HDC dc, int width, int height);
    void DrawPenaltyPointsAndCircle(HDC dc, int width, int height);
    void DrawPlayers(HDC dc, int width, int height);
	void DrawHalfField(HDC dc, int width, int height);
    void DrawPlayersHalf(HDC dc, int width, int height);
    void OnMouseDown(int x, int y);
    void OnMouseMove(int x, int y, WPARAM wParam);
    void OnMouseUp();
    void Resize(int width, int height);
    enum class ViewMode { FullField, HalfField };
    ViewMode currentView = ViewMode::FullField;
    void SetViewMode(ViewMode mode);

private:
    struct Player {
        float xRatio, yRatio;
        bool selected = false;
        int number = 0;
        std::string name;
    };

    struct Ball {
        float xRatio;
        float yRatio;
        bool selected = false;
    };


    HWND hwnd;
    std::vector<Player> players;
    std::vector<Player> opponents;
    Ball ball;
    int selectedBall = -1;
    int selectedIndex = -1;
    int selectedOpponentIndex = -1;
    int windowWidth = 1, windowHeight = 1;

    bool IsInsidePlayer(int x, int y, const Player& p);
    bool IsInsideBall(int mx, int my);

};