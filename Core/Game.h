#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../UI/StatusBar.h" // Include the new header
#include <vector>
#include "../Entities/Animal.h"

class Game
{
private:
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
    StatusBar* pStatusBar; // Pointer for the status bar object  
    std::vector<Animal*> animalList;
    // Game state variables for Task 1
    int currentTimer;
    int currentGoal;
    int currentLevel;

public:
	int budget = 2000;
	Game();
	~Game();

	clicktype getMouseClick(int& x, int& y) const;
	string getSrting() const;

	window* CreateWind(int, int, int, int) const;
	void createToolbar();
	void createBudgetbar();
	void clearBudget() const;
	void printBudget(string msg) const;
	void clearStatusBar() const;

	void printMessage(string msg) const;
	void go(); // Removed 'const' so we can update game state
	window* getWind() const;
};
