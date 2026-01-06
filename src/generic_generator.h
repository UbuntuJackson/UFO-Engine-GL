#pragma once

#include <memory>
#include "actor.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "level.h"

namespace ufo{

class GenericGenerator{
public:
    bool load_for_editor = false;

    //Unused for the time being
    std::vector<std::string> words_blacklisted_as_custom_properties = {
        "x",
        "y",
        "key",
        "name",
        "type"

    };

    std::map<std::string, std::function<std::unique_ptr<Actor>(ufo::gc::JsonMap* _json)>> factory_map;

    virtual void Initialise();
    virtual std::unique_ptr<Actor> FromJson(ufo::gc::JsonMap* _json);
    std::unique_ptr<Actor> FromJsonInGame(ufo::gc::JsonMap* _json);

    std::unique_ptr<Actor> JsonToActorTree(ufo::GarbageCollector* _gc, ufo::gc::JsonMap* _json){

        std::unique_ptr<Actor> actor = FromJson(_json);

        for(ufo::gc::Json* j : _json->AsMap().at("actors")->AsArray()){
            auto j_map = dynamic_cast<ufo::gc::JsonMap*>(j);

            if(j_map) actor->AddActorUniquePtr(JsonToActorTree(_gc, j_map));
            else Console::PrintLine("std::unique_ptr<Actor> Actor::FromJsonToActor: Non-JsonMap item found in json");
        }

        return std::move(actor);

    }

    std::unique_ptr<Level> JsonToLevelTree(ufo::GarbageCollector* _gc, ufo::gc::JsonMap* _json){

        std::unique_ptr<Level> actor = std::make_unique<Level>();
        actor->editor_name = _json->AsMap().at("name")->AsString();

        for(ufo::gc::Json* j : _json->AsMap().at("actors")->AsArray()){
            auto j_map = dynamic_cast<ufo::gc::JsonMap*>(j);

            if(j_map) actor->AddActorUniquePtr(JsonToActorTree(_gc, j_map));
            else Console::PrintLine("std::unique_ptr<Actor> Actor::FromJsonToActor: Non-JsonMap item found in json");
        }

        return std::move(actor);

    }

    virtual ~GenericGenerator() = default;
};

}
