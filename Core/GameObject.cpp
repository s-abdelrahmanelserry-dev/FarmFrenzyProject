#include "GameObject.h"

GameObject::GameObject(Game* r_pGame, point ref, int r_width, int r_height, color fc, color bc) 
    : Drawable(r_pGame, ref, r_width, r_height)
{
    fillColor = fc;
    borderColor = bc;
}

void GameObject::setRefPoint(point p)
{
    RefPoint = p;
}

// Implemented using Axis-Aligned Bounding Box (AABB) collision detection
bool GameObject::CollisionDetection(const GameObject& gObj)
{
    // Object 1 (this) bounds
    int left1 = this->RefPoint.x;
    int right1 = this->RefPoint.x + this->width;
    int top1 = this->RefPoint.y;
    int bottom1 = this->RefPoint.y + this->height;

    // Object 2 (gObj) bounds
    int left2 = gObj.RefPoint.x;
    int right2 = gObj.RefPoint.x + gObj.width;
    int top2 = gObj.RefPoint.y;
    int bottom2 = gObj.RefPoint.y + gObj.height;

    // Check if the rectangles overlap on both the X and Y axes
    // If any of these conditions are true, there is NO collision.
    if (right1 < left2 ||   // 'this' is completely to the left of 'gObj'
        left1 > right2 ||   // 'this' is completely to the right of 'gObj'
        bottom1 < top2 ||   // 'this' is completely above 'gObj'
        top1 > bottom2)     // 'this' is completely below 'gObj'
    {
        return false; 
    }

    // If none of the above are true, they must be overlapping
    return true;
}
