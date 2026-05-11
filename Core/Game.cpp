#include "Game.h"
#include "../Config/GameConfig.h"
#include "../UI/StatusBar.h"

Game::Game()
{
	// 1 - Create the main window
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	// 2 - Task 1: Initialize the Status Bar object
	pStatusBar = new StatusBar();

	// 3 - Initialize game variables with default values
	currentTimer = 120;
	currentGoal = 10;
	currentLevel = 1;

	createToolbar();
	createBudgetbar();
	clearStatusBar();
}

Game::~Game()
{
	// Cleanup pointers to prevent memory leaks
	delete pStatusBar;
	delete gameToolbar;
	delete gameBudgetbar;
	delete pWind;
}

// ... [Existing getMouseClick, getSrting, etc. code] ...

void Game::go()
{
    // ... setup code ...
    do
    {
        // --- TASK 15: MOVE ANIMALS ---
        for (Animal* pAnimal : animalList) {
            if (pAnimal != nullptr) {
                pAnimal->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
            }
        }

        // --- DRAWING SECTION ---
        // 1. Clear the field (Important: Don't clear the UI bars)
        pWind->SetBrush(config.bkGrndColor);
        pWind->SetPen(config.bkGrndColor);
        pWind->DrawRectangle(0, config.toolBarHeight * 2, config.windWidth, config.windHeight - config.statusBarHeight);

        // 2. Draw all animals at their new positions
        for (Animal* pAnimal : animalList) {
            if (pAnimal != nullptr) pAnimal->draw(); 
        }

        // 3. Draw the Status Bar (Task 1)
        pStatusBar->Draw(pWind, currentTimer, currentGoal, currentLevel, animalList.size());

        // ... click handling code ...
        
    } while (!isExit);
}
