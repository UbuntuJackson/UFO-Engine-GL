#pragma once
#include <string>
#include "bit_map_font.h"
#include "widget.h"
#include <unordered_map>
#include "texture_2d.h"
#include "font.h"

namespace ufo{

class Engine;
class Graphics;

class Text : public Widget{
private:

    //std::string text = "Hello world";

    Texture2D texture;

public:
    bool use_bit_map_font = true;
    std::string bit_map_font_key = "res/gfx/unifont.png";
    std::string shader_key = "partial_sprite_shader";
    bool add_bit_map_font = false;
    int character_width = 1;
    int character_height = 1;

    bool is_wrapping = false;

    std::unordered_map<std::string, std::string> language_to_text;

    Font font = Font("../UFO-Engine/res/fonts-japanese-gothic.ttf", 30.0f);

    void SetText(const std::string& _text);

    std::string GetTextFromLanguageMap();

    std::string GetText();

    Text(Vector2f _);

    void OnSpawn() override;

    ~Text();

    void OnIrregularUpdate() override;

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;
    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json) override;

    bool ClickableArea() override;

    void DrawUnscaled(ufo::Graphics *_graphics, ufo::Camera *_camera) override;
    void OnDraw(ufo::Graphics* _graphics, ufo::Camera* _camera) override;

#ifdef UFO_ENGINE_STUDIO
    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

#endif

};

}
