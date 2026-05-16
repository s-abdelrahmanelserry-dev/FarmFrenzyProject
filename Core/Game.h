#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h"
#include <vector>
#include <string>

// هيكل بيانات لتخزين أسماء اللاعبين وسكوراتهم لترتيبها
struct PlayerScore {
    std::string name;
    int score;
};

class Game
{
private:
    window* pWind; 
    Toolbar* gameToolbar;
    Budgetbar* gameBudgetbar;
    StatusBar* pStatusBar;

    std::vector<Animal*> animalList;
    std::vector<Animal*> wolfList;

    float currentTimer;
    int currentGoal;
    int currentLevel;
    int warehouseCount;
    int wolfSpawnTimer;
    bool isGameOver;

    // Task 38: متغيرات نظام لوحة الصدارة واسم المستخدم
    std::string username;
    bool leaderboardUpdated; // لضمان كتابة السكور مرة واحدة فقط عند انتهاء الوقت
    void updateAndShowLeaderboard();

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
    int getWarehouseCount() const { return warehouseCount; }
    
    // Task 39: ميزة إضافية (تحويل منتجات المخزن لكاش نقدى عند الضغط على أرض المزرعة)
    void sellWarehouseProducts();

    void loadGame(std::string filename);
    void spawnWolf();

    window* getWind() const;
};
