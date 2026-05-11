#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"

class StatusBar {
public:
    // Pass the window and the game data here
    void Draw(window* pWind, int timer, int goal, int level, int animals);
};
