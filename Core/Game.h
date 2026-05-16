#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"
#include "../UI/Toolbar.h"
#include "../UI/BudgetBar.h"
#include "../UI/StatusBar.h"
#include "../Entities/Animal.h"
#include <vector>
#include <string>

// هيكل بيانات لتخزين أسماء اللاعبين وسكوراتهم
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

    // Task 38: متغيرات اسم المستخدم ولوحة الصدارة
    std::string username;
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
    
    // Task 39: ميزة إضافية - بيع المنتجات لزيادة الميزانية (Cash Out Bonus)
    void sellWarehouseProducts();

    void loadGame(std::string filename);
    void spawnWolf();

    window* getWind() const;
};
