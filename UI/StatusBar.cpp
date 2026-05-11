#include "StatusBar.h"
#include <string>

void StatusBar::Draw(window* pWind, int timer, int goal, int level, int animals) {
    // 1. Draw the background rectangle at the bottom of the screen
    pWind->SetPen(WHITE, 2);
    pWind->SetBrush(DARKBLUE); // Choose a color that stands out
    
    // Coordinates: (x1, y1) to (x2, y2). 550 to 600 is the bottom 50 pixels.
    pWind->DrawRectangle(0, 550, 800, 600); 

    // 2. Set Text Properties
    pWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWind->SetPen(WHITE);

    // 3. Draw the four required strings
    int textY = 565; // Vertical alignment inside the bar
    pWind->DrawString(20,  textY, "Time: " + std::to_string(timer));
    pWind->DrawString(200, textY, "Goal: " + std::to_string(goal));
    pWind->DrawString(400, textY, "Level: " + std::to_string(level));
    pWind->DrawString(600, textY, "Animals: " + std::to_string(animals));
}
