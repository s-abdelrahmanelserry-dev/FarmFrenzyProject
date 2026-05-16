#include "Game.h"
#include "../Config/GameConfig.h"
#include "../UI/StatusBar.h"
#include <ctime>
#include <fstream>  // مهمة جداً لقراءة الملفات في الـ Load
#include <iostream>

Game::Game()
{
	srand(time(0)); // Seed العشوائية لحركة الحيوانات والولفز

	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	pStatusBar = new StatusBar();
	currentTimer = 120.0f; // بداية العداد من 120 ثانية
	currentGoal = 10;
	currentLevel = 1;
	warehouseCount = 0;
	wolfSpawnTimer = 0;
	isGameOver = false;

	createToolbar();
	createBudgetbar();
	clearStatusBar();
}

Game::~Game()
{
	delete pStatusBar;
	delete gameToolbar;
	delete gameBudgetbar;
	
	// تنظيف الميموري من الحيوانات والولفز لمنع الـ Memory Leak
	for (auto a : animalList) delete a;
	for (auto w : wolfList) delete w;
	
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

void Game::spawnWolf()
{
	point p;
	p.x = rand() % (config.windWidth - 60);
	p.y = (config.toolBarHeight * 2) + (rand() % (config.windHeight - (config.toolBarHeight * 2) - config.statusBarHeight - 60));

	Animal* pWolf = new Animal(this, p, 60, 60, "images\\Wolf.png");
	wolfList.push_back(pWolf);
	printMessage("Warning: A Wolf has appeared!");
}

// Task 29: دالة الـ Load لقراءة حالة اللعبة من ملف خارجي نصي
void Game::loadGame(std::string filename)
{
	std::ifstream inFile(filename);
	if (!inFile) {
		printMessage("Error: Saved file not found!");
		return;
	}

	// 1. مسح الكائنات القديمة من الـ Memory عشان ميتراكموش
	for (auto a : animalList) delete a;
	for (auto w : wolfList) delete w;
	animalList.clear();
	wolfList.clear();

	// 2. قراءة متغيرات اللعبة الأساسية
	inFile >> budget >> currentTimer >> currentLevel >> currentGoal >> warehouseCount;

	// 3. قراءة عدد الحيوانات المحفوظة وبياناتها
	int animalCount;
	inFile >> animalCount;
	for (int i = 0; i < animalCount; i++) {
		int x, y;
		std::string type;
		inFile >> type >> x >> y;
		
		point p;
		p.x = x;
		p.y = y;
		
		if (type == "Chick") {
			animalList.push_back(new Animal(this, p, 50, 50, "images\\chick.jpg"));
		}
	}
	
	inFile.close();
	isGameOver = false; // لو كنا خسرانين وعملنا لود، اللعبة هترجع تشتغل تاني
	printMessage("Game Loaded Successfully!");
}

void Game::go()
{
	int x, y;
	bool isExit = false;
	clock_t lastTime = clock();

	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	do
	{
		// حساب الوقت المنقضي بين كل كادر والتاني (Delta Time)
		clock_t currentTime = clock();
		float deltaTime = float(currentTime - lastTime) / CLOCKS_PER_SEC;
		lastTime = currentTime;

		// --- Task 35: تحديث التايمر وفحص الـ Game Over ---
		if (!isGameOver) {
			currentTimer -= deltaTime;
			if (currentTimer <= 0) {
				currentTimer = 0;
				isGameOver = true;
				printMessage("GAME OVER! Time is Up!");
			}
		}

		// التحديثات والحركة بتشتغل فقط لو الجيم شغال ومش Game Over
		if (!isGameOver) {
			// تحريك وتحديث إنتاج الحيوانات
			for (Animal* pAn : animalList) {
				pAn->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
				pAn->updateProduction(deltaTime);
			}
			// تحريك الديب
			for (Animal* pWolf : wolfList) {
				pWolf->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
			}

			// ظهور عشوائي للـ Wolves
			wolfSpawnTimer++;
			if (wolfSpawnTimer >= 800) {
				spawnWolf();
				wolfSpawnTimer = 0;
			}
		}

		// --- الرندر ومسح الشاشة للمزرعة ---
		pWind->SetBrush(config.bkGrndColor);
		pWind->SetPen(config.bkGrndColor);
		pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

		// رسم المزرعة والإنتاج
		for (Animal* pAn : animalList) {
			pAn->draw();
			if (pAn->getHasProduct()) {
				pWind->SetPen(YELLOW, 2);
				pWind->SetBrush(YELLOW);
				pWind->DrawCircle(pAn->getRefPoint().x + 10, pAn->getRefPoint().y + 10, 8);
			}
		}
		for (Animal* pWolf : wolfList) pWolf->draw();

		// رسم شاشات العرض والبيانات
		pStatusBar->Draw(pWind, (int)currentTimer, currentGoal, currentLevel, animalList.size());
		string budget_string = "BUDGET = $" + to_string(budget) + " | WH: " + to_string(warehouseCount);
		printBudget(budget_string);

		// الـ Input منغير تعطيل حركة الشاشة (Non-blocking input)
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
			else if (!isGameOver) 
			{
				// تجميع المنتجات بالضغط عليها
				for (Animal* pAn : animalList) {
					if (pAn->getHasProduct()) {
						if (x >= pAn->getRefPoint().x && x <= pAn->getRefPoint().x + pAn->getWidth() &&
							y >= pAn->getRefPoint().y && y <= pAn->getRefPoint().y + pAn->getHeight()) {
							
							pAn->collectProduct();
							warehouseCount++;
							printMessage("Collected! Added to Warehouse.");
						}
					}
				}
			}
		}

	} while (!isExit);
}

window* Game::getWind() const
{
	return pWind;
}
