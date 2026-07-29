#pragma once
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "camera.h"
#include "console.h"
#include "graphics.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "ufo_macros.h"

namespace ufo{

class AnimationCluster : public Actor{
public:
    std::vector<Vector2f> positions;

    AnimationCluster(Vector2f _local_position) : Actor(_local_position){
        base_class_name = "ufo::AnimationCluster";
        class_name = base_class_name;
    }

    void OnSpawn() override {

    }

    void Draw(ufo::Graphics* _graphics, ufo::Camera* _camera) override{
        if(actors.size() == 0) return;

        for(const auto& position : positions){

            actors[0]->local_position = position;
            actors[0]->Draw(_graphics, _camera);

        }
    }

#ifdef UFO_ENGINE_STUDIO

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override{
        ufo::gc::JsonMap* this_json = Actor::GetAsJson(_gc);

        gc::JsonArray* arr_positions = _gc->New<gc::JsonArray>();

        for(const auto& pos : positions){
            arr_positions->array.push_back(_gc->New<gc::JsonNumber>(pos.x));
            arr_positions->array.push_back(_gc->New<gc::JsonNumber>(pos.y));
        }

        this_json->map.emplace("positions",arr_positions);

        return this_json;

    }

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json) override{
        if(!_json->map.count("positions")){
            Console::PrintLine(GetInfo()," ",__UFO_PRETTY_FUNCTION__,"Error, no positions found");
            return;
        }

        const auto& arr = _json->map.at("positions")->AsArray();

        for(int i = 0; i < arr.size(); i+=2){
            positions.push_back(Vector2f(arr[i]->AsFloat(),arr[i+1]->AsFloat()));
        }
    }

#endif


};

}
