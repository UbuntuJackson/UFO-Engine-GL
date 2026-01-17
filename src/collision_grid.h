#pragma once
#include <vector>
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

class CollisionGrid : public Actor{
public:
    int column_width = 640;
    int row_height = 640;
    int number_of_divisions = 1;

    std::vector<std::vector<Actor*>> divisions;

    CollisionGrid(Vector2f _);

    int GetDivisionFromPosition(Vector2f _position);

    void OnSpawn();

    void OnUpdate(float _delta_time);

};

}
