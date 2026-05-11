#pragma once
#include <vector>
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h" 

// Forward declaration of Wolf if needed
class Wolf; 

class Game
{
private:
	window* pWind;
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
    StatusBar* pStatusBar;

    std::vector<Animal*> animalList;
    std::vector<Animal*> wolfList; // Task 16: List for wolves

    // Game state
    int currentTimer; // In seconds or frames
    int currentGoal;
    int currentLevel;
    int wolfSpawnInterval; // How often a wolf appears

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
    void spawnWolf(); // Task 16: Logic to create a random wolf

	window* getWind() const;
};
