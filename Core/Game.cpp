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
    wolfSpawnInterval = 500; // Example: Spawn every 500 frames

	createToolbar();
	createBudgetbar();
	clearStatusBar();
}

void Game::spawnWolf()
{
    // Task 16: Generate random wolf based on level
    point p;
    p.x = rand() % (config.windWidth - 50);
    p.y = (config.toolBarHeight * 2) + (rand() % 300);

    // Using Animal class with a wolf image for simplicity, 
    // or a specialized Wolf class if you have one.
    Animal* pWolf = new Animal(this, p, 60, 60, "images\\Wolf.png");
    wolfList.push_back(pWolf);
    printMessage("A Wolf has appeared!");
}

void Game::go()
{
	int x, y;
	bool isExit = false;
    int frameCounter = 0;

	do
	{
        frameCounter++;

        // --- TASK 16: RANDOM WOLF SPAWNING ---
        if (frameCounter % wolfSpawnInterval == 0) {
            spawnWolf();
        }

        // --- TASK 15 & 17: RANDOM MOVEMENT ---
        for (Animal* pAn : animalList) {
            pAn->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
        }
        for (Animal* pWolf : wolfList) {
            pWolf->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
        }

        // --- DRAWING ---
        pWind->SetBrush(config.bkGrndColor);
        pWind->SetPen(config.bkGrndColor);
        pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

        for (Animal* pAn : animalList) pAn->draw();
        for (Animal* pWolf : wolfList) pWolf->draw();

		pStatusBar->Draw(pWind, currentTimer, currentGoal, currentLevel, animalList.size());
		printBudget("BUDGET = $" + to_string(budget));

        // Click Handling
		if (pWind->GetMouseClick(x, y)) // Use non-blocking click for smooth movement
        {
            if (y >= 0 && y < config.toolBarHeight)
                isExit = gameToolbar->handleClick(x, y);
            else if (y >= config.toolBarHeight && y < 2*config.toolBarHeight)
                isExit = gameBudgetbar->handleClick(x, y);
        }

	} while (!isExit);
}

// ... Rest of Game.cpp functions remain unchanged ...
