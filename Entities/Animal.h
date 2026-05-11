#pragma once
#include "GameObject.h"
#include <iostream>

class Animal : public GameObject {
protected:
    int speed;
public:
    Animal(Game* r_pGame, point r_point, int r_width, int r_height, string r_imagePath)
        : GameObject(r_pGame, r_point, r_width, r_height, r_imagePath) {
        speed = 5; // Default movement speed
    }

    void moveRandomly(int windWidth, int windHeight, int toolBarHeight, int statusBarHeight) {
        // 1. Generate random movement (-1, 0, or 1)
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;

        // 2. Calculate potential new position
        int newX = RefPoint.x + (dx * speed);
        int newY = RefPoint.y + (dy * speed);

        // 3. Boundary Checking
        // Stay within horizontal bounds
        if (newX > 0 && newX < windWidth - width) {
            RefPoint.x = newX;
        }
        // Stay below toolbars (2 * toolBarHeight) and above status bar
        if (newY > (2 * toolBarHeight) && newY < windHeight - statusBarHeight - height) {
            RefPoint.y = newY;
        }
    }
};
