#pragma once
#include <string>
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
    bool is_wrapping = false;

    std::unordered_map<std::string, std::string> language_to_text;

    Font font = Font("../UFO-Engine/res/fonts-japanese-gothic.ttf", 30.0f);

    void SetText(const std::string& _text);

    std::string GetTextFromLanguageMap();

    std::string GetText();

    Text(Vector2f _);

    void OnSpawn();

    ~Text();

    void OnIrregularUpdate();

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void OnWidgetDraw(ufo::Graphics* _graphics);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera);

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);

};

}
