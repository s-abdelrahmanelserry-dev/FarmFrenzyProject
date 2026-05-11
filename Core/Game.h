#pragma once
#include <vector>
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h" // Ensure this path is correct

class Game
{
private:
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
    StatusBar* pStatusBar;

    // Task 15: Container for animals
    std::vector<Animal*> animalList;

    // Game state
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

	void go(); // Removed const to allow updating animal positions
    
    // Helper to add animals from UI clicks
    void addAnimal(Animal* pAn) { animalList.push_back(pAn); }

	window* getWind() const;
};
