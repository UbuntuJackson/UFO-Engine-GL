#pragma once
#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"

namespace UFOEngineStudio{
    class Editor;
    class LevelEditorTab;
}

class Camera;

namespace ufo{

class Graphics;

class Widget : public Actor{
public:
    Widget(Vector2f _);

    enum PartsOfRectangle{
        TOP,BOTTOM,RIGHT,LEFT,TOP_LEFT,TOP_RIGHT,BOTTOM_LEFT,BOTTOM_RIGHT,MIDDLE,NONE
    };

    PartsOfRectangle part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;

    ufo::Rectangle rectangle = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(600.0f, 200.0f));

    ufo::Rectangle GetRectangle();

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void ResizeOrMove(Vector2f _position_screen_space,Vector2f _size_screen_space, Vector2f& _rectangle_position, Vector2f& _rectangle_size, Vector2f _screen_space_mouse_position);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json);
    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);
};

}
