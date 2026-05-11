#include "StatusBar.h"
#include <string>

void StatusBar::Draw(window* pWind, int timer, int goal, int level, int animals) {
    // 1. Draw the background bar (so text is readable)
    pWind->SetPen(WHITE, 3);
    pWind->SetBrush(DARKSLATEGRAY); // Or any color you like
    // Assuming a 600px height window, bar starts at 550
    pWind->DrawRectangle(0, 550, 800, 600); 

    // 2. Set text style
    pWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWind->SetPen(WHITE);

    // 3. Draw the 4 required strings
    pWind->DrawString(20, 565, "Time: " + std::to_string(timer));
    pWind->DrawString(200, 565, "Goal: " + std::to_string(goal));
    pWind->DrawString(400, 565, "Level: " + std::to_string(level));
    pWind->DrawString(600, 565, "Animals: " + std::to_string(animals));
}
