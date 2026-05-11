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
	int x, y;
	bool isExit = false;

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	do
	{
		// --- TASK 1: UPDATE THE STATUS BAR EVERY LOOP ---
		// We pass the current state to the Draw function
		// '0' is a placeholder for the animal count until animal logic is added
		pStatusBar->Draw(pWind, currentTimer, currentGoal, currentLevel, 0); 

		string budget_string = "BUDGET = $" + to_string(budget);
		printBudget(budget_string);

		getMouseClick(x, y);	

		if (y >= 0 && y < config.toolBarHeight)
		{
			isExit = gameToolbar->handleClick(x, y);
		}
		else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
		{
			isExit = gameBudgetbar->handleClick(x, y);
		}

	} while (!isExit);
}
