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

    std::vector<Animal*> animalList;
    std::vector<Animal*> wolfList;

    // Game state
    int currentTimer;
    int currentGoal;
    int currentLevel;
    int wolfSpawnTimer;

    // Task 20: Warehouse tracking
    int warehouseCount; 

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

	void go(); 
    void addAnimal(Animal* pAn) { animalList.push_back(pAn); }
    void spawnWolf();

    // Task 20: Getter for warehouse
    int getWarehouseCount() const { return warehouseCount; }

	window* getWind() const;
};
