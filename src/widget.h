#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "graphics.h"

#ifdef UFO_ENGINE_STUDIO
#include "viewport_editing_utils.h"
namespace UFOEngineStudio{
    class Editor;
    class LevelEditorTab;
}
#endif

namespace ufo{

class Camera;
class Graphics;

class Widget : public Actor{
public:
    bool visible = true;
    std::string texture_key = "placeholder_icon";
    Vector2f offset;
    float current_frame_index = 0;
    float number_of_frames = 1;
    Vector2f frame_size = {32.0f, 32.0f};
    Vector2f scale = {1.0f, 1.0f};
    ufo::Colour tint = ufo::Colour(255,255,255,255);
    std::string shader_key = "partial_sprite_shader";
    float corner_rounding = 0.0f;

    Widget(Vector2f _);

    ufo::Rectangle GetRectangle();

    void OnDraw(ufo::Graphics* _graphics, ufo::Camera* _camera) override;

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json);
    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);

    #ifdef UFO_ENGINE_STUDIO

    PartsOfRectangle part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;
    void OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnResourcesEdited() override;

    #endif
};

}
