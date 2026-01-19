#pragma once
#include <vector>
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

class CollisionGrid : public Actor{
public:
    int column_width = 160;
    int row_height = 160;
    int number_of_divisions = 1;
    int number_of_columns = 1;
    int number_of_rows = 1;

    enum Errors{
        OUT_OF_BOUNDS = -1
    };

    std::vector<std::vector<Actor*>> divisions;

    CollisionGrid(Vector2f _);

    int GetDivisionFromPosition(const Vector2f& _position);

    void OnSpawn();

    void OnUpdate(float _delta_time);

};

}
