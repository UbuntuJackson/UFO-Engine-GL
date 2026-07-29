#pragma once

#include "../ufo_maths/ufo_maths.h"
#include "actor.h"

namespace ufo{
    class GarbageCollector;
    namespace gc{
        class JsonMap;
    }
}

namespace UFOEngineStudio{
    class LevelEditorTab;
}

namespace ufo{

struct Bounds{
    float x0;
    float x1;
    float y0;
    float y1;
    float GetWidthHalf(){return (x1-x0)/2.0f;} //something is wrong if this returns negative
    float GetHeightHalf(){return (y1-y0)/2.0f;} //something is wrong if this returns negative
};

class Camera : public Actor{
public:

    bool camera_moves_independently = true;

    //Unused for now
    Vector2f original_position;

    bool clamp = true;
    Bounds view;
    Bounds world;
    float rotation;
    float scale;

    //Force camera to follow specific local position
    bool follow_initial_local_position = false;

    //For GL
    ufo::Rectangle viewport = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));

    Camera(olc::vf2d _position);
    void OnSpawn() override;
    void EarlyUpdate();
    void ClampLocalPosition();
    void OnUpdate(float _delta_time) override;
    void HandleUpdate();
    void OnKilled() override;

    olc::vf2d Transform(const olc::vf2d& _position);
    olc::vf2d TransformScreenToWorld(const olc::vf2d& _screen_position);
    //SpriteReference Transform(const SpriteReference& _sprite);
    bool IsOnScreen(olc::vf2d _position, olc::vf2d _offset = {0.0f, 0.0f});
    ufo::Rectangle GetOnScreenRectangleInWorld(olc::vf2d _offset = {0.0f, 0.0f});

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;
    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json) override;

#ifdef UFO_ENGINE_STUDIO

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

#endif

};

}
