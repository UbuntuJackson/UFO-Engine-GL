#pragma once
#include <unordered_map>
#include "../ufo_maths/ufo_maths.h"
#include "sprite.h"

namespace ufo::gc
{
    class JsonMap;
}

namespace UFOEngineStudio{
    class LevelEditorTab;
}

namespace ufo{

class GarbageCollector;
class Graphics;

class Animation : public Sprite{
public:

    float animation_speed = 100.0f;
    //int cycle_count = 0;
    //float frame_counter = 0.0f;

    struct Costume{
        std::string key;
        //To be swapped for a variable called position, to avoid inconsistencies with local_position.
        olc::vf2d local_position;
        olc::vf2d offset;
        olc::vf2d frame_size;
        olc::vf2d scale;
        float rotation;
        float frame_index;
        float animation_speed;
    };

    bool preview = false;

    //Make a special type of Sprite with overwritten Draw function to not have the OnDraw callback call by default (scrapped idea)
    //Sprite* current_animation = nullptr;

    std::unordered_map<std::string, Costume> costumes;

    Animation(Vector2f _local_position);

    void SetCostume(const std::string& _costume_key);

    void AddCostume(
        std::string _key,
        olc::vf2d _local_position,
        olc::vf2d _offset,
        olc::vf2d _frame_size,
        olc::vf2d _scale,
        float _rotation, float _frame_index, float _animation_speed);

    void OnSpawn() override;

    void OnUpdate(float _delta_time) override;

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;
    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json) override;

#ifdef UFO_ENGINE_STUDIO

    void OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

    void OnResourcesEdited() override;

#endif //UFO_ENGINE_STUDIO

};

}
