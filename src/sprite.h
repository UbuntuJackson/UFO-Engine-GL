#pragma once
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "graphics.h"
#include "shader.h"

namespace ufo{

class Graphics;
class Camera;

class Sprite : public Actor{
public:
    std::string key;
    Vector2f offset;
    Vector2f frame_size;
    Vector2f scale = {1.0f, 1.0f};
    float rotation = 0.0f;
    float current_frame_index = 0.0f;
    bool visible = true;
    ufo::Colour tint = olc::WHITE;

    float number_of_frames = 1.0f;

    float corner_rounding = 0.0f;

    //Custom shader can either use std::string, raw int or the shader class instance.
    std::string shader_key = "partial_sprite_shader";

    Sprite(olc::vf2d _position, std::string _key, olc::vf2d _offset, olc::vf2d _frame_size, olc::vf2d _scale, float _rotation, float _frame_index);

    Sprite(olc::vf2d _position);

    void OnSpawn() override;

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera) override;

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json) override;

#ifdef UFO_ENGINE_STUDIO

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

    ImVec4 im_colour = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    void OnAdditionalButtonsForTreeItem() override;

    //For UFO-Engine Studio

    void OnResourcesEdited() override;
#endif //UFO_ENGINE_STUDIO
};

}
