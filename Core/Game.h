#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h"
#include <vector>
#include <string>

class Game
{
private:
	window* pWind;	// Pointer to the CMU graphics window
	Toolbar* gameToolbar;
	Budgetbar* gameBudgetbar;
	StatusBar* pStatusBar;

	// الإضافات الخاصة بإدارة الكائنات وحالة اللعبة
	std::vector<Animal*> animalList;
	std::vector<Animal*> wolfList;

	float currentTimer; // خليناه float عشان نطرح منه بدقة الأجزاء من الثانية
	int currentGoal;
	int currentLevel;
	int warehouseCount;
	int wolfSpawnTimer;
	bool isGameOver;    // Flag لمتابعة نهاية الوقت

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

	// شيلنا الـ const من دالة go عشان تقدر تغير الوقت والمخزن والفلوس داخلياً
	void go();


	void addAnimal(Animal* pAn) { animalList.push_back(pAn); }
	int getWarehouseCount() const { return warehouseCount; }
	
	// Task 29:Load  ملف 
	void loadGame(std::string filename);
	void spawnWolf();

	window* getWind() const;		
};
