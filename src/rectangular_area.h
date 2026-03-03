#pragma once

#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../shapes/rectangle.h"
#include "animation.h"
#include "gc_json.h"
#include "im_vec.h"
#include "viewport_editing_utils.h"
#include "../ufo_engine_studio/level_editor_tab.h"

namespace ufo{

class Camera;
namespace gc{
    class GarbageCollector;
    class JsonMap;
}

class RectangularArea : public Actor{
public:
    RectangularArea(Vector2f _);

    ufo::Rectangle rectangle = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(100.0f, 50.0f));

    PartsOfRectangle part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json);

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);

};

}
