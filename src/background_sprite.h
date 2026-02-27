#pragma once

#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "gc_json.h"
#include "sprite.h"

namespace UFOEngineStudio{class LevelEditorTab;}

namespace ufo{

namespace gc{class JsonMap;}

class Graphics;
class Camera;

class BackgroundSprite : public Sprite{
public:
    Vector2f parallax = Vector2f(0.5f,0.5f);

    BackgroundSprite(Vector2f _local_position);

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera) override;

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json) override;

};

}
