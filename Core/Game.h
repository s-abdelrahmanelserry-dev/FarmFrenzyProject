#pragma once
#include <vector>
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h" 

class Game
{
private:
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
    StatusBar* pStatusBar;

    std::vector<Animal*> animalList; // For player-owned animals
    std::vector<Animal*> wolfList;   // Task 16: For enemy wolves

    // Game state variables
    int currentTimer;
    int currentGoal;
    int currentLevel;
    int wolfSpawnTimer; // Counter to track when to spawn a wolf

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

	void go(); // Task 17: Update loop for movement
    void addAnimal(Animal* pAn) { animalList.push_back(pAn); }
    void spawnWolf(); // Task 16: Random wolf generation

	window* getWind() const;
};
