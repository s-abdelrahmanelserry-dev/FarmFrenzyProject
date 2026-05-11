#include "Game.h"
#include "../Config/GameConfig.h"
#include "../UI/StatusBar.h"
#include <ctime>

// ... [Constructor and Destructor remain the same] ...

void Game::go()
{
	int x, y;
	bool isExit = false;
    
    // For Task 19: Tracking time
    clock_t lastTime = clock();

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy - - - - - - - - - -");

	do
	{
        // Calculate deltaTime (seconds passed since last loop)
        clock_t currentTime = clock();
        float deltaTime = float(currentTime - lastTime) / CLOCKS_PER_SEC;
        lastTime = currentTime;

        // --- TASK 15 & 17: MOVE & TASK 19: PRODUCE ---
        for (Animal* pAn : animalList) {
            pAn->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
            pAn->updateProduction(deltaTime); // Update production timer
        }

        for (Animal* pWolf : wolfList) {
            pWolf->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
        }

        // --- RENDERING ---
        pWind->SetBrush(config.bkGrndColor);
        pWind->SetPen(config.bkGrndColor);
        pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

        for (Animal* pAn : animalList) {
            pAn->draw();
            // Visual indicator for Task 19 (e.g., draw a small circle if product is ready)
            if (pAn->getHasProduct()) {
                pWind->SetPen(YELLOW, 2);
                pWind->SetBrush(YELLOW);
                pWind->DrawCircle(pAn->getRefPoint().x + 10, pAn->getRefPoint().y + 10, 5);
            }
        }
        for (Animal* pWolf : wolfList) pWolf->draw();

        // --- UI ---
		pStatusBar->Draw(pWind, currentTimer, currentGoal, currentLevel, animalList.size());
		printBudget("BUDGET = $" + to_string(budget));

		if (pWind->GetMouseClick(x, y)) 
        {
            if (y >= 0 && y < config.toolBarHeight)
                isExit = gameToolbar->handleClick(x, y);
            else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
                isExit = gameBudgetbar->handleClick(x, y);
            else {
                // Task 20: Check if player clicked an animal to collect product
                for (Animal* pAn : animalList) {
                    if (x >= pAn->getRefPoint().x && x <= pAn->getRefPoint().x + pAn->getWidth() &&
                        y >= pAn->getRefPoint().y && y <= pAn->getRefPoint().y + pAn->getHeight()) {
                        if (pAn->getHasProduct()) {
                            pAn->collectProduct();
                            printMessage("Product Collected!");
                        }
                    }
                }
            }
        }

	} while (!isExit);
}
