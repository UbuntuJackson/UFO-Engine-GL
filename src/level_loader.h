#pragma once
#include <string>
#include <memory>
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "engine.h"

namespace ufo{

class LevelLoader : public ufo::gc::Root{
public:
    LevelLoader() = default;
     std::unique_ptr<Actor> LoadLevel(ufo::Engine* _engine, const std::string& _level){
        auto level_json = ufo::gc::JsonRead(&gc, _level);
        if(level_json->IsNull()){
            throw std::runtime_error("[UFO-Engine] LevelLoader::LoadLevel: Could not load level "+ _level+".");
        }

        auto level = _engine->actor_generator->JsonToActorTree(&gc,level_json);
        return level;
     }
};

}
