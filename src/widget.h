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

    ufo::Rectangle rectangle = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(600.0f, 200.0f));

    ufo::Rectangle GetRectangle();

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void ResizeOrMove(Vector2f _screen_space_mouse_position);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);
};

}
