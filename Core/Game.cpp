#include "Game.h"
#include "../Config/GameConfig.h"
#include <ctime>

Game::Game()
{
    srand(time(0)); 
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	pStatusBar = new StatusBar();
	currentTimer = 120;
	currentGoal = 10;
	currentLevel = 1;
    wolfSpawnTimer = 0;
    warehouseCount = 0; // Initialize warehouse at 0

	createToolbar();
	createBudgetbar();
	clearStatusBar();
}

// ... [Destructor and existing helpers] ...

void Game::go()
{
	int x, y;
	bool isExit = false;
    clock_t lastTime = clock();

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy - - - - - - - - - -");

	do
	{
        // 1. Time tracking for Task 19 Production
        clock_t currentTime = clock();
        float deltaTime = float(currentTime - lastTime) / CLOCKS_PER_SEC;
        lastTime = currentTime;

        // 2. Logic Updates (Movement & Production)
        for (Animal* pAn : animalList) {
            pAn->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
            pAn->updateProduction(deltaTime);
        }
        for (Animal* pWolf : wolfList) {
            pWolf->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
        }

        // 3. Spawning Logic
        wolfSpawnTimer++;
        if (wolfSpawnTimer >= (1000 / currentLevel)) {
            spawnWolf();
            wolfSpawnTimer = 0;
        }

        // 4. Drawing Everything
        pWind->SetBrush(config.bkGrndColor);
        pWind->SetPen(config.bkGrndColor);
        pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

        for (Animal* pAn : animalList) {
            pAn->draw();
            // Draw the product (Task 19)
            if (pAn->getHasProduct()) {
                pWind->SetPen(YELLOW, 2);
                pWind->SetBrush(YELLOW);
                pWind->DrawCircle(pAn->getRefPoint().x + 10, pAn->getRefPoint().y + 10, 8);
            }
        }
        for (Animal* pWolf : wolfList) pWolf->draw();

        // 5. Task 1: Update Status Bar (Passing warehouseCount as animal count for now or adding a 5th param)
		pStatusBar->Draw(pWind, currentTimer, currentGoal, currentLevel, animalList.size());
		printBudget("BUDGET = $" + to_string(budget) + " | WH: " + to_string(warehouseCount));

        // 6. Task 20: Input & Collection Logic
		if (pWind->GetMouseClick(x, y)) 
        {
            if (y >= 0 && y < config.toolBarHeight)
                isExit = gameToolbar->handleClick(x, y);
            else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
                isExit = gameBudgetbar->handleClick(x, y);
            else {
                // Check if user clicked a product on an animal
                for (Animal* pAn : animalList) {
                    if (pAn->getHasProduct()) {
                        // Simple Box Collision Detection
                        if (x >= pAn->getRefPoint().x && x <= pAn->getRefPoint().x + pAn->getWidth() &&
                            y >= pAn->getRefPoint().y && y <= pAn->getRefPoint().y + pAn->getHeight()) {
                            
                            pAn->collectProduct(); // Task 19: Reset flag
                            warehouseCount++;     // Task 20: Increment storage
                            printMessage("Product stored in Warehouse!");
                        }
                    }
                }
            }
        }

	} while (!isExit);
}
