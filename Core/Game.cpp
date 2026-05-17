#include "Game.h"
#include "../Config/GameConfig.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h"
#include "../Entities/Chick.h" 
#include "../Entities/Cow.h"   
#include "../Entities/Wolf.h"  
#include <ctime>
#include <fstream>  
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;

// Helper functions for field boundaries
static int WPX() { return config.windWidth - config.warehousePanelWidth; }
static int FL() { return config.fieldMargin; }
static int FR() { return config.windWidth - config.warehousePanelWidth - config.fieldMargin; }
static int FT() { return 2 * config.toolBarHeight + config.fieldMargin; }
static int FB() { return config.windHeight - config.statusBarHeight - config.fieldMargin; }

Game::Game()
{
	srand(time(0)); 

	// Set current working directory to executable path
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
	currentGoal = 5000;
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

	// Clear legacy arrays used as fallback 
	for (int i = 0; i < MAX_PRODUCTS; i++) eggList[i]  = nullptr;
	for (int i = 0; i < MAX_PRODUCTS; i++) milkList[i] = nullptr;
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

	gameBudgetbar = new Budgetbar(this, budgetbarUpperleft, 0, config.toolBarHeight);
	gameBudgetbar->draw();
}

void Game::drawBackground() const
{
	int fieldTop    = 2 * config.toolBarHeight;
	int fieldBottom = config.windHeight - config.statusBarHeight;
	int fieldLeft   = 0;
	int fieldRight  = config.windWidth;
	int margin      = 30;

	pWind->SetBrush(GREEN);
	pWind->SetPen(GREEN, 1);
	pWind->DrawRectangle(fieldLeft, fieldTop, fieldRight, fieldBottom);

	pWind->SetBrush(BROWN);
	pWind->SetPen(BROWN, 1);
	pWind->DrawRectangle(fieldLeft + margin, fieldTop + margin, fieldRight - margin, fieldBottom - margin);
	
	pWind->SetPen(DARKORANGE, 4);
	pWind->DrawRectangle(fieldLeft + margin, fieldTop + margin, fieldRight - margin, fieldBottom - margin, FRAME);
	
	int postHalf = 6;
	pWind->SetBrush(DARKORANGE);
	pWind->SetPen(DARKORANGE, 1);
	pWind->DrawRectangle(fieldLeft  + margin - postHalf, fieldTop    + margin - postHalf, fieldLeft  + margin + postHalf, fieldTop    + margin + postHalf);
	pWind->DrawRectangle(fieldRight - margin - postHalf, fieldTop    + margin - postHalf, fieldRight - margin + postHalf, fieldTop    + margin + postHalf);
	pWind->DrawRectangle(fieldLeft  + margin - postHalf, fieldBottom - margin - postHalf, fieldLeft  + margin + postHalf, fieldBottom - margin + postHalf);
	pWind->DrawRectangle(fieldRight - margin - postHalf, fieldBottom - margin - postHalf, fieldRight - margin + postHalf, fieldBottom - margin + postHalf);
}

void Game::updateFoodCounter() const
{
	int areaX = config.windWidth - 220;
	int areaY = 2 * config.toolBarHeight + 5;

	pWind->SetPen(GREEN, 1);
	pWind->SetBrush(GREEN);
	pWind->DrawRectangle(areaX - 5, areaY - 5, config.windWidth - 5, areaY + 42);

	if (foodCount > 0)
	{
		pWind->SetPen(WHITE, 2);
		pWind->SetFont(20, BOLD, BY_NAME, "Arial");
		string foodMsg = "Food: " + to_string(foodCount);
		pWind->DrawString(areaX, areaY + 8, foodMsg);
	}
}

void Game::clearBudget() const
{
	pWind->SetPen(config.bkGrndColor, 1);
	pWind->SetBrush(config.bkGrndColor);
	pWind->DrawRectangle(config.windWidth - 500, config.toolBarHeight, config.windWidth, 2 * config.toolBarHeight);
}

void Game::printBudget(string msg) const
{
	clearBudget();	
	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(config.windWidth - 350, config.toolBarHeight + 10, msg);
}

void Game::clearStatusBar() const
{
	pWind->SetPen(config.statusBarColor, 1);
	pWind->SetBrush(config.statusBarColor);
	pWind->DrawRectangle(0, config.windHeight - config.statusBarHeight, config.windWidth, config.windHeight);
}

void Game::printMessage(string msg) const
{
	clearStatusBar();	
	pWind->SetPen(config.penColor, 50);
	pWind->SetFont(24, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10, config.windHeight - (int)(0.85 * config.statusBarHeight), msg);
}

void Game::drawStatusBar() const
{
	clearStatusBar();
	int y = config.windHeight - (int)(0.75 * config.statusBarHeight);
	pWind->SetPen(WHITE, 1);
	pWind->SetFont(16, BOLD, BY_NAME, "Arial");
	pWind->DrawString(10,  y, "Time: "    + to_string(timer));
	pWind->DrawString(130, y, "Level: "   + to_string(level));
	pWind->DrawString(240, y, "Goal: $" + to_string(currentGoal));
	pWind->DrawString(430, y, "Animals: " + to_string(animalList.size()));
}

void Game::spawnWolf()
{
	point p;
	p.x = FL() + rand() % (FR() - FL() - config.wolfWidth);
	p.y = FT() + rand() % (FB() - FT() - config.wolfHeight);

	Animal* pWolf = new Wolf(this, p, config.wolfWidth, config.wolfHeight, "images\\wolf.jpg");
	wolfList.push_back(pWolf);
	wolfClicks.push_back(0); 

	printMessage("Warning: A Wolf has appeared!");
}

void Game::loadGame()
{
	ifstream f(config.saveFileName);
	if (!f.is_open()) { printMessage("ERROR: No save file found!"); return; }
	
	restartGame();
	string token;
	
	while (f >> token)
	{
		if (token == "LEVEL")  { 
			f >> level; 
			currentLevel = level;
		}
		else if (token == "BUDGET") { f >> budget; }
		else if (token == "TIMER")  { 
			f >> timer; 
			currentTimer = (float)timer;
		}
		else if (token == "ANIMALS")
		{
			int n; f >> n;
			for (int i = 0; i < n; i++)
			{
				string type; int px, py; 
				f >> type >> px >> py;
				point p; p.x = px; p.y = py;
				if (type == "CHICK") { 
					animalList.push_back(new Chick(this, p, config.animalWidth, config.animalHeight, "images\\chick.jpg")); 
					chickCount++;
				}
				else { 
					animalList.push_back(new Cow(this, p, config.animalWidth, config.animalHeight, "images\\cow.jpg"));
					cowCount++;   
				}
			}
		}
		else if (token == "WOLVES")
		{
			int n; f >> n;
			for (int i = 0; i < n; i++)
			{
				string lbl; int px, py;
				f >> lbl >> px >> py;
				point p; p.x = px; p.y = py;
				wolfList.push_back(new Wolf(this, p, config.wolfWidth, config.wolfHeight, "images\\wolf.jpg"));
				wolfClicks.push_back(0);
			}
		}
		else if (token == "EGGS") { f >> warehouseEggs; }
		else if (token == "MILK") { f >> warehouseMilk; }
	}
	f.close();
	
	drawBackground(); 
	drawWarehousePanel();
	clearBudget(); 
	printBudget("BUDGET = $" + to_string(budget));
	updateFoodCounter(); 
	drawStatusBar();
	printMessage("Game Loaded!");
}

void Game::saveGame() const
{
	ofstream f(config.saveFileName);
	if (!f.is_open()) { printMessage("ERROR: Could not save!"); return; }
	
	f << "LEVEL "  << level  << "\n" << "BUDGET " << budget << "\n" << "TIMER " << timer << "\n";
	
	f << "ANIMALS " << animalList.size() << "\n";
	for (size_t i = 0; i < animalList.size(); i++)
	{
		if (!animalList[i]) continue;
		Chick* ch = dynamic_cast<Chick*>(animalList[i]);
		f << (ch ? "CHICK" : "COW") << " " << animalList[i]->getRefPoint().x << " " << animalList[i]->getRefPoint().y << "\n";
	}
	
	f << "WOLVES " << wolfList.size() << "\n";
	for (size_t i = 0; i < wolfList.size(); i++) 
	{
		if (wolfList[i])
			f << "WOLF " << wolfList[i]->getRefPoint().x << " " << wolfList[i]->getRefPoint().y << "\n";
	}
	
	f << "WAREHOUSE\n" << "EGGS " << warehouseEggs << "\n" << "MILK " << warehouseMilk << "\n";
	f.close();
	printMessage("Game Saved!");
}

void Game::restartGame()
{
	for (auto a : animalList) delete a;
	for (auto w : wolfList) delete w;
	animalList.clear();
	wolfList.clear();
	wolfClicks.clear();

	for (int i = 0; i < eggFieldCount;  i++) { delete eggList[i]; eggList[i] = nullptr; }
	for (int i = 0; i < milkFieldCount; i++) { delete milkList[i]; milkList[i] = nullptr; }
	
	animalCount = wolfCount = eggFieldCount = milkFieldCount = 0;
	budget = 2000; 
	foodCount = 10;
	chickCount = cowCount = 0;
	level = 1; 
	currentLevel = 1;
	currentTimer = 120.0f;
	timer = 120; 
	paused = false;
	isGameOver = false;
	warehouseEggs = warehouseMilk = warehouseCount = 0;
	
	drawBackground();
	drawWarehousePanel();
	clearBudget();
	printBudget("BUDGET = $" + to_string(budget));
	updateFoodCounter();
	drawStatusBar();
	printMessage("Game Restarted!");
}

void Game::go()
{
	int x, y;
	bool isExit = false;
	clock_t lastTime = clock();

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");
	spawnWolf();

	do
	{
		clock_t currentTime = clock();
		float deltaTime = float(currentTime - lastTime) / CLOCKS_PER_SEC;
		lastTime = currentTime;

		if (!isGameOver && !paused) {
			currentTimer -= deltaTime;
			timer = (int)currentTimer;
			if (currentTimer <= 0) {
				currentTimer = 0;
				timer = 0;
				isGameOver = true;
				printMessage("GAME OVER! Time is Up!");
			}
		}

		if (!isGameOver && !paused) {
			for (Animal* pAn : animalList) {
				pAn->moveRandomly(WPX(), config.windHeight, config.toolBarHeight, config.statusBarHeight);
				pAn->updateProduction(deltaTime);
			}
			for (Animal* pWolf : wolfList) {
				pWolf->moveRandomly(WPX(), config.windHeight, config.toolBarHeight, config.statusBarHeight);
			}

			// AABB Collision Detection and response mechanics (Wolf eating animals)
			for (size_t wIdx = 0; wIdx < wolfList.size(); wIdx++) {
				Animal* w = wolfList[wIdx];
				
				for (auto animIt = animalList.begin(); animIt != animalList.end(); ) {
					Animal* a = *animIt;
					
					if (w->getRefPoint().x < a->getRefPoint().x + a->getWidth() &&
						w->getRefPoint().x + w->getWidth() > a->getRefPoint().x &&
						w->getRefPoint().y < a->getRefPoint().y + a->getHeight() &&
						w->getRefPoint().y + w->getHeight() > a->getRefPoint().y) {
						
						delete a;
						animIt = animalList.erase(animIt); 
						printMessage("Oh no! A wolf attacked and killed an animal!");
					} else {
						++animIt;
					}
				}
			}

			wolfSpawnTimer++;
			if (wolfSpawnTimer >= 800) {
				spawnWolf();
				wolfSpawnTimer = 0;
			}
		}

		// Render Pipeline 
		pWind->SetBrush(config.bkGrndColor);
		pWind->SetPen(config.bkGrndColor);
		pWind->DrawRectangle(0, 2 * config.toolBarHeight, WPX(), config.windHeight - config.statusBarHeight);
		
		drawBackground();
		updateFoodCounter();
		drawWarehousePanel();

		for (Animal* pAn : animalList) {
			pAn->draw();
			if (pAn->getHasProduct()) {
				pWind->SetPen(YELLOW, 2);
				pWind->SetBrush(YELLOW);
				pWind->DrawCircle(pAn->getRefPoint().x + 10, pAn->getRefPoint().y + 10, 8);
			}
		}
		
		for (int i = 0; i < eggFieldCount; i++) {
			if (eggList[i] && !eggList[i]->collected) {
				pWind->DrawImage("images\\egg.png", eggList[i]->getRefPoint().x, eggList[i]->getRefPoint().y, config.productWidth, config.productHeight);
			}
		}
		
		for (int i = 0; i < milkFieldCount; i++) {
			if (milkList[i] && !milkList[i]->collected) {
				pWind->DrawImage("images\\milk.png", milkList[i]->getRefPoint().x, milkList[i]->getRefPoint().y, config.productWidth, config.productHeight);
			}
		}

		for (Animal* pWolf : wolfList) pWolf->draw();

		drawStatusBar();
		string budget_string = "BUDGET = $" + to_string(budget) + " | WH: " + to_string(warehouseCount);
		printBudget(budget_string);

		// Non-blocking asynchronous input handling loop
		if (pWind->GetMouseClick(x, y)) 
		{
			if (y >= 0 && y < config.toolBarHeight)
			{
				isExit = gameToolbar->handleClick(x, y);
			}
			else if (y >= config.toolBarHeight && y < 2 * config.toolBarHeight)
			{
				isExit = gameBudgetbar->handleClick(x, y);
			}
			else if (x >= WPX())
			{
				showWarehouse();
			}
			else if (!isGameOver) 
			{
				bool targetInteracted = false;

				// Wolf targeting mechanics
				for (size_t i = 0; i < wolfList.size(); i++) {
					Animal* pWolf = wolfList[i];
					if (x >= pWolf->getRefPoint().x && x <= pWolf->getRefPoint().x + config.wolfWidth &&
						y >= pWolf->getRefPoint().y && y <= pWolf->getRefPoint().y + config.wolfHeight) {
						
						wolfClicks[i]++; 
						printMessage("Wolf Hit! Clicks: " + to_string(wolfClicks[i]) + "/5");
						targetInteracted = true;

						if (wolfClicks[i] >= 5) {
							printMessage("Great Job! The wolf has been defeated!");
							delete pWolf;
							wolfList.erase(wolfList.begin() + i);
							wolfClicks.erase(wolfClicks.begin() + i);
						}
						break; 
					}
				}

				// Field drop collection handling (Eggs)
				if (!targetInteracted) {
					for (int i = 0; i < eggFieldCount; i++) {
						if (eggList[i] && !eggList[i]->collected) {
							int ex = eggList[i]->getRefPoint().x;
							int ey = eggList[i]->getRefPoint().y;
							if (x >= ex && x <= ex + config.productWidth && y >= ey && y <= ey + config.productHeight) {
								eggList[i]->collected = true;
								warehouseEggs++;
								warehouseCount++;
								printMessage("Egg collected! Total: " + to_string(warehouseEggs));
								targetInteracted = true;
								break;
							}
						}
					}
				}

				// Field drop collection handling (Milk)
				if (!targetInteracted) {
					for (int i = 0; i < milkFieldCount; i++) {
						if (milkList[i] && !milkList[i]->collected) {
							int mx = milkList[i]->getRefPoint().x;
							int my = milkList[i]->getRefPoint().y;
							if (x >= mx && x <= mx + config.productWidth && y >= my && y <= my + config.productHeight) {
								milkList[i]->collected = true;
								warehouseMilk++;
								warehouseCount++;
								printMessage("Milk collected! Total: " + to_string(warehouseMilk));
								targetInteracted = true;
								break;
							}
						}
					}
				}

				// Fallback generic collection 
				if (!targetInteracted) {
					for (Animal* pAn : animalList) {
						if (pAn->getHasProduct()) {
							if (x >= pAn->getRefPoint().x && x <= pAn->getRefPoint().x + config.animalWidth &&
								y >= pAn->getRefPoint().y && y <= pAn->getRefPoint().y + config.animalHeight) {
								
								pAn->collectProduct();
								warehouseCount++;
								
								if (dynamic_cast<Chick*>(pAn)) warehouseEggs++;
								else if (dynamic_cast<Cow*>(pAn)) warehouseMilk++;

								printMessage("Collected! Added to Warehouse.");
							}
						}
					}
				}
			}
		}

	} while (!isExit);

	stopBackgroundMusic();
}

void Game::drawWarehousePanel() const
{
	int px = WPX();
	int py = 2 * config.toolBarHeight;
	int pw = config.warehousePanelWidth;
	int ph = config.windHeight - py - config.statusBarHeight;

	// Background drawing
	pWind->SetBrush(BEIGE);
	pWind->SetPen(SADDLEBROWN, 3);
	pWind->DrawRectangle(px, py, px + pw, py + ph);

	// Title Section Bar
	pWind->SetBrush(SADDLEBROWN);
	pWind->SetPen(SADDLEBROWN, 1);
	pWind->DrawRectangle(px, py, px + pw, py + 30);
	pWind->SetPen(WHITE, 1);
	pWind->SetFont(15, BOLD, BY_NAME, "Arial");
	pWind->DrawString(px + 35, py + 8, "WAREHOUSE");

	// Eggs Segment Status
	int ey = py + 40;
	pWind->SetBrush(LIGHTYELLOW);
	pWind->SetPen(DARKORANGE, 1);
	pWind->DrawRectangle(px + 5, ey, px + pw - 5, ey + 70);
	pWind->DrawImage("images\\egg.png", px + 8, ey + 5, 45, 55);
	pWind->SetPen(BLACK, 1);
	pWind->SetFont(13, BOLD,  BY_NAME, "Arial");
	pWind->DrawString(px + 60, ey + 8,  "EGGS");
	pWind->SetFont(12, PLAIN, BY_NAME, "Arial");
	pWind->DrawString(px + 60, ey + 24, "Count: " + to_string(warehouseEggs));
	pWind->DrawString(px + 60, ey + 40, "Price: $" + to_string(config.eggPrice));
	pWind->DrawString(px + 60, ey + 56, "Total: $" + to_string(warehouseEggs * config.eggPrice));

	// Milk Segment Status
	int my = ey + 80;
	pWind->SetBrush(LIGHTYELLOW);
	pWind->SetPen(DARKORANGE, 1);
	pWind->DrawRectangle(px + 5, my, px + pw - 5, my + 70);
	pWind->DrawImage("images\\milk.png", px + 8, my + 5, 45, 55);
	pWind->SetPen(BLACK, 1);
	pWind->SetFont(13, BOLD,  BY_NAME, "Arial");
	pWind->DrawString(px + 60, my + 8,  "MILK");
	pWind->SetFont(12, PLAIN, BY_NAME, "Arial");
	pWind->DrawString(px + 60, my + 24, "Count: " + to_string(warehouseMilk));
	pWind->DrawString(px + 60, my + 40, "Price: $" + to_string(config.milkPrice));
	pWind->DrawString(px + 60, my + 56, "Total: $" + to_string(warehouseMilk * config.milkPrice));

	// Panel Budget Tracking
	int by = my + 90;
	pWind->SetPen(DARKGREEN, 2);
	pWind->SetBrush(LIGHTGREEN);
	pWind->DrawRectangle(px + 5, by, px + pw - 5, by + 35);
	pWind->SetPen(DARKGREEN, 1);
	pWind->SetFont(12, BOLD, BY_NAME, "Arial");
	pWind->DrawString(px + 10, by + 5,  "Budget:");
	pWind->DrawString(px + 10, by + 20, "$" + to_string(budget));

	// Interactive Popup Overlay Trigger Button
	int btnY = by + 45;
	pWind->SetBrush(DARKGREEN);
	pWind->SetPen(GREEN, 2);
	pWind->DrawRectangle(px + 10, btnY, px + pw - 10, btnY + 35);
	pWind->SetPen(WHITE, 1);
	pWind->SetFont(13, BOLD, BY_NAME, "Arial");
	pWind->DrawString(px + 22, btnY + 10, "Open & Sell");
}

void Game::showWarehouse()
{
	int ww = 380, wh = 320;
	window* whWind = new window(ww, wh, config.wx + 200, config.wy + 100);

	auto redraw = [&]()
	{
		whWind->SetBrush(BEIGE);
		whWind->SetPen(SADDLEBROWN, 3);
		whWind->DrawRectangle(0, 0, ww, wh);

		whWind->SetBrush(SADDLEBROWN);
		whWind->SetPen(SADDLEBROWN, 1);
		whWind->DrawRectangle(0, 0, ww, 35);
		whWind->SetPen(WHITE, 1);
		whWind->SetFont(16, BOLD, BY_NAME, "Arial");
		whWind->DrawString(ww/2 - 65, 8, "=== WAREHOUSE ===");

		// Egg row interactive sell box
		whWind->SetBrush(LIGHTYELLOW);
		whWind->SetPen(DARKORANGE, 2);
		whWind->DrawRectangle(10, 45, ww - 10, 125);
		whWind->DrawImage("images\\egg.png", 15, 50, 55, 65);
		whWind->SetPen(BLACK, 1);
		whWind->SetFont(14, BOLD,  BY_NAME, "Arial");
		whWind->DrawString(80, 52, "EGGS");
		whWind->SetFont(13, PLAIN, BY_NAME, "Arial");
		whWind->DrawString(80, 70,  "Count : " + to_string(warehouseEggs));
		whWind->DrawString(80, 87,  "Price : $" + to_string(config.eggPrice) + " each");
		whWind->DrawString(80, 104, "Total : $" + to_string(warehouseEggs * config.eggPrice));
		whWind->SetBrush(GREEN);
		whWind->SetPen(DARKGREEN, 2);
		whWind->DrawRectangle(ww - 105, 60, ww - 15, 110);
		whWind->SetPen(WHITE, 1);
		whWind->SetFont(14, BOLD, BY_NAME, "Arial");
		whWind->DrawString(ww - 90, 72, "SELL");
		whWind->DrawString(ww - 90, 88, "EGGS");

		// Milk row interactive sell box
		whWind->SetBrush(LIGHTYELLOW);
		whWind->SetPen(DARKORANGE, 2);
		whWind->DrawRectangle(10, 135, ww - 10, 215);
		whWind->DrawImage("images\\milk.png", 15, 140, 55, 65);
		whWind->SetPen(BLACK, 1);
		whWind->SetFont(14, BOLD,  BY_NAME, "Arial");
		whWind->DrawString(80, 142, "MILK");
		whWind->SetFont(13, PLAIN, BY_NAME, "Arial");
		whWind->DrawString(80, 160, "Count : " + to_string(warehouseMilk));
		whWind->DrawString(80, 177, "Price : $" + to_string(config.milkPrice) + " each");
		whWind->DrawString(80, 194, "Total : $" + to_string(warehouseMilk * config.milkPrice));
		whWind->SetBrush(GREEN);
		whWind->SetPen(DARKGREEN, 2);
		whWind->DrawRectangle(ww - 105, 150, ww - 15, 200);
		whWind->SetPen(WHITE, 1);
		whWind->SetFont(14, BOLD, BY_NAME, "Arial");
		whWind->DrawString(ww - 90, 162, "SELL");
		whWind->DrawString(ww - 90, 178, "MILK");

		// Statistics updates and window close command area
		whWind->SetPen(DARKGREEN, 1);
		whWind->SetFont(14, BOLD, BY_NAME, "Arial");
		whWind->DrawString(15, 228, "Current Budget: $" + to_string(budget));
		whWind->SetBrush(RED);
		whWind->SetPen(DARKRED, 2);
		whWind->DrawRectangle(ww/2 - 50, 260, ww/2 + 50, 300);
		whWind->SetPen(WHITE, 1);
		whWind->DrawString(ww/2 - 30, 273, "CLOSE");
	};

	redraw();

	bool open = true;
	while (open)
	{
		int cx, cy;
		whWind->WaitMouseClick(cx, cy);
		if (cx >= ww-105 && cx <= ww-15 && cy >= 60 && cy <= 110)
		{
			if (warehouseEggs > 0)
			{
				budget += warehouseEggs * config.eggPrice;
				warehouseEggs = 0;
				warehouseCount -= warehouseEggs; 
				redraw();
				drawWarehousePanel();
				clearBudget();
				printBudget("BUDGET = $" + to_string(budget));
				printMessage("Eggs sold!");
			}
			else printMessage("No eggs in warehouse!");
		}
		else if (cx >= ww-105 && cx <= ww-15 && cy >= 150 && cy <= 200)
		{
			if (warehouseMilk > 0)
			{
				budget += warehouseMilk * config.milkPrice;
				warehouseMilk = 0;
				warehouseCount -= warehouseMilk;
				redraw();
				drawWarehousePanel();
				clearBudget();
				printBudget("BUDGET = $" + to_string(budget));
				printMessage("Milk sold!");
			}
			else printMessage("No milk in warehouse!");
		}
		else if (cx >= ww/2-50 && cx <= ww/2+50 && cy >= 260 && cy <= 300)
		{
			open = false;
		}
	}
	delete whWind;
}


void Game::playBackgroundMusic() const
{
	mciSendString(TEXT("close bgm"), NULL, 0, NULL);
	MCIERROR err = mciSendString(
		TEXT("open \"sounds\\background.mp3\" type mpegvideo alias bgm"),
		NULL, 0, NULL
	);
	if (err == 0)
	{
		mciSendString(TEXT("play bgm repeat"), NULL, 0, NULL);
	}
	else
	{
		mciSendString(
			TEXT("open \"sounds\\background.wav\" type waveaudio alias bgm"),
			NULL, 0, NULL
		);
		mciSendString(TEXT("play bgm repeat"), NULL, 0, NULL);
	}
}

void Game::stopBackgroundMusic() const
{
	mciSendString(TEXT("stop bgm"),  NULL, 0, NULL);
	mciSendString(TEXT("close bgm"), NULL, 0, NULL);
}

window* Game::getWind() const
{
	return pWind;
}
