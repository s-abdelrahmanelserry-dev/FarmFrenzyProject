#pragma once
#include "../Core/Drawable.h"

class Animal :public Drawable
{
private:
	string image_path;
public:
	point curr_pos;
	point curr_vel;
	Animal(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void draw() const override;
	virtual void moveStep() = 0;   //The action that should be taken each time step
    void moveRandomly(int windWidth, int windHeight, int toolBarHeight, int statusBarHeight) {
        // 1. Pick a random direction (-1, 0, or 1 for both X and Y)
        int dx = (rand() % 3) - 1; 
        int dy = (rand() % 3) - 1;
        
        int speed = 5; // You can adjust this value

        // 2. Update position
        int newX = RefPoint.x + (dx * speed);
        int newY = RefPoint.y + (dy * speed);

        // 3. Boundary Checking (Keep them in the field)
        // Ensure they stay below the toolbar/budgetbar and above the status bar
        if (newX > 0 && newX < windWidth - width) {
            RefPoint.x = newX;
        }
        if (newY > toolBarHeight * 2 && newY < windHeight - statusBarHeight - height) {
            RefPoint.y = newY;
        }
    }
};

class Chick : public Animal
{
public:
	Chick(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
};

class Cow : public Animal
{
public:
	Cow(Game* r_pGame, point r_point, int r_width, int r_height, string img_path);
	virtual void moveStep();
};
