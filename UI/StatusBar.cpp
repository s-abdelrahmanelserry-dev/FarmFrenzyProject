#include "StatusBar.h"
#include <string>

void StatusBar::Draw(window* pWind, int timer, int goal, int level, int animals) {
    // 1. Draw the background rectangle at the bottom of the screen
    // Using standard colors from the library
    pWind->SetPen(WHITE, 2);
    pWind->SetBrush(DARKBLUE); 
    
    // Coordinates assume a standard window; adjustment might be needed based on GameConfig.h
    // (x1, y1) to (x2, y2)
    pWind->DrawRectangle(0, 550, 800, 600); 

    // 2. Set Text Properties
    pWind->SetFont(18, BOLD, BY_NAME, "Arial");
    pWind->SetPen(WHITE);

    // 3. Draw the four required strings as per Task 1 requirements
    int textY = 565; 
    pWind->DrawString(20,  textY, "Time: " + std::to_string(timer));
    pWind->DrawString(200, textY, "Goal: " + std::to_string(goal));
    pWind->DrawString(400, textY, "Level: " + std::to_string(level));
    pWind->DrawString(600, textY, "Animals: " + std::to_string(animals));
}
