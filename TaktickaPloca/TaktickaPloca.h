#pragma once
#include <vector>
#include <windows.h>
#include "gdi.h"
#include <string>

class TaktickaPloca {
public:
    enum class ViewMode { FullField, HalfField };
    enum class Mode { Normal, AddLine };

    struct Player {
        float xRatio, yRatio;
        bool selected = false;
    };

    struct Line {
        struct {
            float xRatio, yRatio;
        } start, end;
    };

    TaktickaPloca();

    void Resize(int w, int h);
    void SetViewMode(ViewMode mode);
    void Draw(HDC hdc);

    void OnMouseDown(int x, int y);
    bool OnMouseMove(int x, int y, WPARAM wParam);
    void OnMouseUp();

    void SetMode(Mode mode);
    bool IsInAddLineMode() const;

    void OnLineStart(int x, int y);
    void OnLineEnd(int x, int y);

    bool SaveTactic(const std::wstring& filename) const;
    bool LoadTactic(const std::wstring& filename);

private:
    std::vector<Player> players;
    std::vector<Player> opponents;
    Player ball;

    Mode currentMode = Mode::Normal;

    std::vector<Line> lines;
    bool drawingLine = false;
    POINT lineStart;

    int selectedIndex = -1;
    int selectedOpponentIndex = -1;
    int selectedBall = -1;

    int windowWidth = 0;
    int windowHeight = 0;
    int width = 0;
    int height = 0;
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
    void DrawLines(HDC dc);
    void ResetPositions();
    void InitPositions();
};