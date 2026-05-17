#include "Game.h"
#include "../Config/GameConfig.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h"
#include <ctime>
#include <fstream>  
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

// Helper functions for field boundaries from file 2
static int WPX() { return config.windWidth - config.warehousePanelWidth; }
static int FL() { return config.fieldMargin; }
static int FR() { return config.windWidth - config.warehousePanelWidth - config.fieldMargin; }
static int FT() { return 2 * config.toolBarHeight + config.fieldMargin; }
static int FB() { return config.windHeight - config.statusBarHeight - config.fieldMargin; }

Game::Game()
{
	srand(time(0)); 

	// Set current working directory to executable path (from file 2)
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);
	string exeDir(exePath);
	exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));
	SetCurrentDirectoryA(exeDir.c_str());

	// Initialize window
	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	pStatusBar = new StatusBar();
	
	// Game state variables initialization
	currentTimer = 120.0f; 
	timer = 120;
	currentGoal = 10;
	currentLevel = 1;
	level = 1;
	budget = 2000;
	foodCount = 10;
	warehouseCount = 0;
	warehouseEggs = 0;
	warehouseMilk = 0;
	chickCount = 0;
	cowCount = 0;
	wolfSpawnTimer = 0;
	isGameOver = false;
	paused = false;

	// Clear array pointers if arrays are used as fallback
	for (int i = 0; i < MAX_ANIMALS;  i++) animalListArray[i] = nullptr;
	for (int i = 0; i < MAX_WOLVES;   i++) wolfListArray[i]   = nullptr;
	for (int i = 0; i < MAX_PRODUCTS; i++) eggList[i]    = nullptr;
	for (int i = 0; i < MAX_PRODUCTS; i++) milkList[i]   = nullptr;
	animalCount = 0;
	wolfCount = 0;
	eggFieldCount = 0;
	milkFieldCount = 0;

	// UI Component creation
	createToolbar();
	createBudgetbar();
	drawBackground();
	drawWarehousePanel();
	clearStatusBar();

	// Start background music
	playBackgroundMusic();
}

Game::~Game()
{
	delete pStatusBar;
	delete gameToolbar;
	delete gameBudgetbar;
	
	// Clean up vector containers
	for (auto a : animalList) delete a;
	for (auto w : wolfList) delete w;
	
	// Clean up array containers
	for (int i = 0; i < animalCount;    i++) delete animalListArray[i];
	for (int i = 0; i < wolfCount;      i++) delete wolfListArray[i];
	for (int i = 0; i < eggFieldCount;  i++) delete eggList[i];
	for (int i = 0; i < milkFieldCount; i++) delete milkList[i];

	stopBackgroundMusic();
	delete pWind;
}

clicktype Game::getMouseClick(int& x, int& y) const
{
	return pWind->WaitMouseClick(x, y);	
}

string Game::getSrting() const
{
	string Label;
	char Key;
	keytype ktype;
	pWind->FlushKeyQueue();
	while (1)
	{
		ktype = pWind->WaitKeyPress(Key);
		if (ktype == ESCAPE)	
			return "";	
		if (Key == 13)	
			return Label;
		if (Key == 8)	
			if (Label.size() > 0)
				Label.resize(Label.size() - 1);
			else
				Key = '\0';
		else
			Label += Key;
		printMessage(Label);
	}
}

window* Game::CreateWind(int w, int h, int x, int y) const
{
	window* pW = new window(w, h, x, y);
	pW->SetBrush(config.bkGrndColor);
	pW->SetPen(config.bkGrndColor, 1);
	pW->DrawRectangle(0, 0, w, h);
	return pW;
}

void Game::createToolbar() 
{
	point toolbarUpperleft;
	toolbarUpperleft.x = 0;
	toolbarUpperleft.y = 0;

	gameToolbar = new Toolbar(this, toolbarUpperleft, 0, config.toolBarHeight);
	gameToolbar->draw();
}

void Game::createBudgetbar()
{
	point budgetbarUpperleft;
	budgetbarUpperleft.x = 0;
	budgetbarUpperleft.y = config.toolBarHeight;

	gameBudgetbar = new Budget
