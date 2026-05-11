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

	createToolbar();
	createBudgetbar();
	clearStatusBar();
}

Game::~Game()
{
	delete pStatusBar;
    for(auto a : animalList) delete a;
    for(auto w : wolfList) delete w;
	delete gameToolbar;
	delete gameBudgetbar;
	delete pWind;
}

// Task 16: Generate a random wolf based on current level and time
void Game::spawnWolf()
{
    point p;
    // Random X across the screen
    p.x = rand() % (config.windWidth - 60);
    // Random Y within the field area (below toolbars, above status bar)
    p.y = (config.toolBarHeight * 2) + (rand() % (config.windHeight - (config.toolBarHeight * 2) - config.statusBarHeight - 60));

    // Create wolf using the Animal class structure with a wolf image
    Animal* pWolf = new Animal(this, p, 60, 60, "images\\Wolf.png");
    wolfList.push_back(pWolf);
    printMessage("Warning: A Wolf has appeared!");
}

void Game::go()
{
	int x, y;
	bool isExit = false;

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy - - - - - - - - - -");

	do
	{
        // --- TASK 16: WOLF SPAWNING LOGIC ---
        wolfSpawnTimer++;
        // Spawn interval decreases as level increases (example logic)
        if (wolfSpawnTimer >= (1000 / currentLevel)) {
            spawnWolf();
            wolfSpawnTimer = 0;
        }

        // --- TASK 15 & 17: RANDOM MOVEMENT ---
        // Move Animals
        for (Animal* pAn : animalList) {
            pAn->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
        }
        // Move Wolves
        for (Animal* pWolf : wolfList) {
            pWolf->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
        }

        // --- RENDERING ---
        // 1. Clear playing field
        pWind->SetBrush(config.bkGrndColor);
        pWind->SetPen(config.bkGrndColor);
        pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

        // 2. Draw Animals
        for (Animal* pAn : animalList) pAn->draw();
        
        // 3. Draw Wolves
        for (Animal* pWolf : wolfList) pWolf->draw();

        // 4. Draw UI Elements
		pStatusBar->Draw(pWind, currentTimer, currentGoal, currentLevel, animalList.size());
		printBudget("BUDGET = $" + to_string(budget));

        // Use GetMouseClick (non-blocking) instead of WaitMouseClick to allow animation
		if (pWind->GetMouseClick(x, y)) 
        {
            if (y >= 0 && y < config.toolBarHeight)
                isExit = gameToolbar->handleClick(x, y);
            else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
                isExit = gameBudgetbar->handleClick(x, y);
        }

	} while (!isExit);
}
