#pragma once
#include "../CMUgraphicsLib/CMUgraphics.h"

class Game;     // Forward declaration

struct point
{
	int x, y;
};

class Drawable
{
protected:
	point RefPoint;     // Each game object must have a reference point
	int width, height;
	Game* pGame;        // Pointer to game object

public:
	Drawable(Game* r_pGame, point r_point, int r_width, int r_height);
	virtual void draw() const = 0;

	// Getters - needed to access position from outside the class
	point getRefPoint() const { return RefPoint; }
	int   getWidth()    const { return width; }
	int   getHeight()   const { return height; }
};
