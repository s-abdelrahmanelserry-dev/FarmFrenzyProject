#include "Budgetbar.h"
#include "../Config/GameConfig.h"
#include "../Core/Game.h"
#include "../Entities/Animal.h" // Ensure this is included to recognize Chick/Animal classes
#include <iostream>
#include <random> // For mt19937

using namespace std;

BudgetbarIcon::BudgetbarIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : Drawable(r_pGame, r_point, r_width, r_height)
{
	image_path = img_path;
}

void BudgetbarIcon::draw() const
{
	window* pWind = pGame->getWind();
	pWind->DrawImage(image_path, RefPoint.x, RefPoint.y, width, height);
}

ChickIcon::ChickIcon(Game* r_pGame, point r_point, int r_width, int r_height, string img_path) : BudgetbarIcon(r_pGame, r_point, r_width, r_height, img_path)
{
    // Initialize count for tracking chicks locally if needed
    count = 0; 
}

void ChickIcon::onClick()
{
	cout << "Icon Chick Clicked" << endl;

	// Check if player has enough budget (Chick costs 100)
	if (pGame->budget >= 100) {
		pGame->budget = pGame->budget - 100;
		
		// Update the Budget display on screen
		pGame->clearBudget();
		string budget_string = "BUDGET = $" + to_string(pGame->budget);
		pGame->printBudget(budget_string);

		// --- TASK 15: SPAWN CHICK AT RANDOM POSITION ---
		point p;
		std::random_device rd;
		std::mt19937 gen(rd());
		
		// Define spawn range within the field boundaries
		// Min Y should be below the toolbars (2 * toolbarHeight)
		// Max Y should be above the status bar
		std::uniform_int_distribution<int> distX(50, config.windWidth - 50);
		std::uniform_int_distribution<int> distY(config.toolBarHeight * 2 + 20, config.windHeight - config.statusBarHeight - 70);

		p.x = distX(gen);
		p.y = distY(gen);

		// Create the new Chick object
		// We use a size of 50x50 as per your startup code
		Animal* new_chick = new Animal(pGame, p, 50, 50, "images\\chick.jpg");
		
		// IMPORTANT: Add the chick to the Game's master list
		// This allows the Game::go() loop to move and draw it automatically
		pGame->addAnimal(new_chick); 
		
		cout << "New Chick added to game at (" << p.x << "," << p.y << ")" << endl;
	}
	else {
		pGame->printMessage("Not enough budget!");
	}
}

Budgetbar::Budgetbar(Game* r_pGame, point r_point, int r_width, int r_height) : Drawable(r_pGame, r_point, r_width, r_height)
{
	iconsImages[ICON_CHICK] = "images\\chick.jpg";

	point p;
	p.x = 0;
	p.y = config.toolBarHeight;

	iconsList = new BudgetbarIcon * [ANIMAL_COUNT];

	iconsList[ICON_CHICK] = new ChickIcon(pGame, p, config.iconWidth, config.toolBarHeight, iconsImages[ICON_CHICK]);
}

Budgetbar::~Budgetbar()
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		delete iconsList[i];
	delete[] iconsList;
}

void Budgetbar::draw() const
{
	for (int i = 0; i < ANIMAL_COUNT; i++)
		iconsList[i]->draw();
		
	window* pWind = pGame->getWind();
	pWind->SetPen(BLACK, 3);
	// Draw a separator line between BudgetBar and the Playing Field
	pWind->DrawLine(0, 2 * config.toolBarHeight, pWind->GetWidth(), 2 * config.toolBarHeight);
}

bool Budgetbar::handleClick(int x, int y)
{
	if (x > ANIMAL_COUNT * config.iconWidth)
		return false;

	int clickedIconIndex = (x / config.iconWidth);
	iconsList[clickedIconIndex]->onClick();

	return false;
}
