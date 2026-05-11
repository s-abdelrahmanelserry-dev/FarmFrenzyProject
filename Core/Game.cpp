#include "Game.h"
#include "../Config/GameConfig.h"
#include "../UI/StatusBar.h"
#include <ctime>

Game::Game()
{
    srand(time(0)); // Seed randomness for animal movement
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	pStatusBar = new StatusBar();
	currentTimer = 120;
	currentGoal = 10;
	currentLevel = 1;

	createToolbar();
	createBudgetbar();
	clearStatusBar();
}

Game::~Game()
{
	delete pStatusBar;
    for(auto a : animalList) delete a;
	delete gameToolbar;
	delete gameBudgetbar;
	delete pWind;
}

// ... (getMouseClick and other helper functions remain the same) ...

void Game::go()
{
	int x, y;
	bool isExit = false;

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	do
	{
        // --- TASK 15: UPDATE ANIMALS ---
        for (Animal* pAn : animalList) {
            pAn->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
        }

        // --- DRAWING ---
        // 1. Clear playing area (Between BudgetBar and StatusBar)
        pWind->SetBrush(config.bkGrndColor);
        pWind->SetPen(config.bkGrndColor);
        pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

        // 2. Draw Animals
        for (Animal* pAn : animalList) {
            pAn->draw();
        }

        // 3. Draw UI
		pStatusBar->Draw(pWind, currentTimer, currentGoal, currentLevel, animalList.size());
		string budget_string = "BUDGET = $" + to_string(budget);
		printBudget(budget_string);

        // Check for clicks (Note: Non-blocking check is better for animation, 
        // but sticking to your current structure)
		getMouseClick(x, y);	

		if (y >= 0 && y < config.toolBarHeight)
		{
			isExit = gameToolbar->handleClick(x, y);
		}
		else if (y >= config.toolBarHeight && y < 2*config.toolBarHeight)
		{
			isExit = gameBudgetbar->handleClick(x, y);
		}

	} while (!isExit);
}

// ... (remaining window functions) ...
