#pragma once
#include <vector>
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"

namespace UFOEngineStudio{
    class LevelEditorTab;
}

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

    std::vector<int> GetDivisionsForRectangle(const ufo::Rectangle& _rect);

    void OnSpawn();

    void OnUpdate(float _delta_time);
#ifdef UFO_ENGINE_STUDIO
    Actor* OnGetFocusedActor(Vector2f _mouse_position_over_screenspace) override;

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);
#endif

};

}
