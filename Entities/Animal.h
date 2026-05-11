#pragma once
#include "GameObject.h"
#include <iostream>

class Animal : public GameObject {
protected:
    int speed;
    float productionCounter; // Tracks time until next product
    float productionMax;     // Time required to produce (e.g., 10.0 for Chicken)
    bool hasProduct;         // Flag to check if a product is ready to be collected

public:
    Animal(Game* r_pGame, point r_point, int r_width, int r_height, string r_imagePath)
        : GameObject(r_pGame, r_point, r_width, r_height, r_imagePath) {
        speed = 5;
        productionCounter = 0;
        productionMax = 10.0; // Default 10 seconds as per Task 19
        hasProduct = false;
    }

    // Task 15 & 17: Random Movement
    void moveRandomly(int windWidth, int windHeight, int toolBarHeight, int statusBarHeight) {
        int dx = (rand() % 3) - 1;
        int dy = (rand() % 3) - 1;
        int newX = RefPoint.x + (dx * speed);
        int newY = RefPoint.y + (dy * speed);

        if (newX > 0 && newX < windWidth - width) RefPoint.x = newX;
        if (newY > (2 * toolBarHeight) && newY < windHeight - statusBarHeight - height) RefPoint.y = newY;
    }

    // Task 19: Production Logic
    void updateProduction(float deltaTime) {
        if (!hasProduct) {
            productionCounter += deltaTime;
            if (productionCounter >= productionMax) {
                hasProduct = true;
                productionCounter = 0;
                // You can add logic here to spawn a visual product object later
                std::cout << "Animal produced an item!" << std::endl;
            }
        }
    }

    bool getHasProduct() const { return hasProduct; }
    void collectProduct() { hasProduct = false; }
};
