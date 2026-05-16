#include "Game.h"
#include "../Config/GameConfig.h"
#include "../UI/StatusBar.h"
#include <ctime>
#include <fstream>  
#include <iostream>
#include <algorithm> // مهم جداً من أجل std::sort لترتيب لوحة الصدارة

Game::Game()
{
	srand(time(0)); 

	pWind = CreateWind(config.windWidth, config.windHeight, config.wx, config.wy);

	pStatusBar = new StatusBar();
	currentTimer = 120.0f; 
	currentGoal = 10;
	currentLevel = 1;
	warehouseCount = 0;
	wolfSpawnTimer = 0;
	isGameOver = false;
	
	// Task 38: تهيئة المتغيرات الافتراضية
	username = "Guest";
	leaderboardUpdated = false;

	createToolbar();
	createBudgetbar();
	clearStatusBar();
}

Game::~Game()
{
	delete pStatusBar;
	delete gameToolbar;
	delete gameBudgetbar;
	
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
	pWind->DrawString(config.windWidth - 450, config.toolBarHeight + 10, msg); // تم زيادة المساحة لتعرض الإسم والميزانية بوضوح
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

// Task 38: دالة تحديث السكور وحفظ وترتيب لوحة الصدارة في ملف نصي خارجي
void Game::updateAndShowLeaderboard()
{
	std::vector<PlayerScore> scoreBoard;
	std::ifstream inFile("leaderboard.txt");

	// 1. قراءة البيانات المسجلة سابقاً إن وجدت
	if (inFile.is_open()) {
		PlayerScore temp;
		while (inFile >> temp.name >> temp.score) {
			scoreBoard.push_back(temp);
		}
		inFile.close();
	}

	// 2. حساب وتثبيت سكور اللاعب الحالي (الميزانية + بونص المنتجات غير المباعة)
	int currentScore = budget + (warehouseCount * 30);
	scoreBoard.push_back({ username, currentScore });

	// 3. ترتيب السكور تنازلياً باستخدام الـ Lambda Function وأداة std::sort
	std::sort(scoreBoard.begin(), scoreBoard.end(), [](const PlayerScore& a, const PlayerScore& b) {
		return a.score > b.score;
	});

	// 4. حفظ أفضل 5 أرقام قياسية فقط لحماية حجم الملف
	std::ofstream outFile("leaderboard.txt");
	if (outFile.is_open()) {
		int limit = std::min((int)scoreBoard.size(), 5);
		for (int i = 0; i < limit; i++) {
			outFile << scoreBoard[i].name << " " << scoreBoard[i].score << "\n";
		}
		outFile.close();
	}

	// 5. طباعة النتيجة النهائية وصاحب المركز الأول على الـ Status Bar
	std::string topPlayerMsg = "Top 1: " + scoreBoard[0].name + " ($" + std::to_string(scoreBoard[0].score) + ")";
	printMessage("GAME OVER! Your Score: " + std::to_string(currentScore) + " | " + topPlayerMsg);
}

// Task 39: Bonus Feature - دالة بيع محتويات المخزن فوراً لزيادة السيولة وشراء المزيد من الدواجن
void Game::sellWarehouseProducts()
{
	if (warehouseCount > 0) {
		int cashEarned = warehouseCount * 50; // كل منتج يمنح 50 دولار كاش
		budget += cashEarned;
		printMessage("Market Sale! Sold " + std::to_string(warehouseCount) + " items. Gained: $" + std::to_string(cashEarned));
		warehouseCount = 0; // تصفير المخزن بعد إتمام البيع بنجاح
	} else {
		printMessage("Warehouse is empty! Produce more items first.");
	}
}

void Game::loadGame(std::string filename)
{
	std::ifstream inFile(filename);
	if (!inFile) {
		printMessage("Error: Saved file not found!");
		return;
	}

	for (auto a : animalList) delete a;
	for (auto w : wolfList) delete w;
	animalList.clear();
	wolfList.clear();

	inFile >> budget >> currentTimer >> currentLevel >> currentGoal >> warehouseCount;

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
	isGameOver = false; 
	leaderboardUpdated = false; // إعادة السماح بتحديث الليدربورد عند انتهاء هذا الدور الجديد
	printMessage("Game Loaded Successfully!");
}

void Game::go()
{
	int x, y;
	bool isExit = false;

	// --- Task 38: طلب إدخال اسم اللاعب فور تشغيل اللعبة وقبل بدء الـ Loop الرئيسي ---
	printMessage("Type Username then press ENTER: ");
	username = getSrting();
	if (username.empty()) {
		username = "Player1"; // اسم افتراضي لحماية البرنامج في حال ضغط اللاعب إدخال مباشر
	}
	printMessage("Welcome " + username + "! Farm is Ready...");

	clock_t lastTime = clock();
	pWind->ChangeTitle("- - - - - - - - - - Farm Frenzy (CIE101-project) - - - - - - - - - -");

	do
	{
		clock_t currentTime = clock();
		float deltaTime = float(currentTime - lastTime) / CLOCKS_PER_SEC;
		lastTime = currentTime;

		// تحديث التايمر وفحص الـ Game Over
		if (!isGameOver) {
			currentTimer -= deltaTime;
			if (currentTimer <= 0) {
				currentTimer = 0;
				isGameOver = true;
			}
		}

		// التحكم في تشغيل اللعبة وحفظ الليدربورد
		if (!isGameOver) {
			for (Animal* pAn : animalList) {
				pAn->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
				pAn->updateProduction(deltaTime);
			}
			for (Animal* pWolf : wolfList) {
				pWolf->moveRandomly(config.windWidth, config.windHeight, config.toolBarHeight, config.statusBarHeight);
			}

			wolfSpawnTimer++;
			if (wolfSpawnTimer >= 800) {
				spawnWolf();
				wolfSpawnTimer = 0;
			}
		} 
		else {
			// عند الـ Game Over نقوم بتحديث الـ Leaderboard لمرة واحدة فقط دون تكرار
			if (!leaderboardUpdated) {
				updateAndShowLeaderboard();
				leaderboardUpdated = true;
			}
		}

		// --- الرندر ومسح الشاشة للمزرعة ---
		pWind->SetBrush(config.bkGrndColor);
		pWind->SetPen(config.bkGrndColor);
		pWind->DrawRectangle(0, 2 * config.toolBarHeight, config.windWidth, config.windHeight - config.statusBarHeight);

		for (Animal* pAn : animalList) {
			pAn->draw();
			if (pAn->getHasProduct()) {
				pWind->SetPen(YELLOW, 2);
				pWind->SetBrush(YELLOW);
				pWind->DrawCircle(pAn->getRefPoint().x + 10, pAn->getRefPoint().y + 10, 8);
			}
		}
		for (Animal* pWolf : wolfList) pWolf->draw();

		pStatusBar->Draw(pWind, (int)currentTimer, currentGoal, currentLevel, animalList.size());
		
		// إدراج اسم اللاعب النشط حالياً في لوحة الميزانية العلوية
		string budget_string = "PLAYER: " + username + " | BUDGET = $" + to_string(budget) + " | WH: " + to_string(warehouseCount);
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
				bool productClicked = false;
				
				// تجميع المنتجات بالضغط عليها
				for (Animal* pAn : animalList) {
					if (pAn->getHasProduct()) {
						if (x >= pAn->getRefPoint().x && x <= pAn->getRefPoint().x + pAn->getWidth() &&
							y >= pAn->getRefPoint().y && y <= pAn->getRefPoint().y + pAn->getHeight()) {
							
							pAn->collectProduct();
							warehouseCount++;
							printMessage("Collected! Added to Warehouse.");
							productClicked = true;
							break;
						}
					}
				}

				// Task 39: الميزة المبتكرة (Smart Gestures Interface)
				// إذا نقر اللاعب داخل المزرعة ولم يكن يضغط على منتج، يتم استدعاء دالة البيع الفوري تلقائياً لتسهيل اللعب وتحويل المخزن لسيولة نقدية لشراء حيوانات جديدة
				if (!productClicked) {
					sellWarehouseProducts();
				}
			}
		}

	} while (!isExit);
}

window* Game::getWind() const
{
	return pWind;
}
