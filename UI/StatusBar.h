#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"

class StatusBar {
public:
    // Draws the bar background and the four required game stats
    void Draw(window* pWind, int timer, int goal, int level, int animals);
};
