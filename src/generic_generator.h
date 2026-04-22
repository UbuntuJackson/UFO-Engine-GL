#pragma once

#include <memory>
#include "actor.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../shared_json/shared_json.h"
#include "level.h"
#include "object.h"

namespace ufo{

class Engine;

class GenericGenerator : public ufo::gc::Root{
public:
    GenericGenerator(){
        name = "GC_GenericGenerator";
    }

    std::map<std::string, ufo::gc::JsonMap*> actor_jsons_with_unaltered_default_properties;
    std::map<std::string, ufo::gc::JsonMap*> class_jsons;
    std::map<std::string, std::string> inheritence_map;

    std::string GetBaseClassOf(std::string _class_name);

    void InitialiseActorClassJsons(const std::string& _game_directory);

    void OnMark() {
        for(const auto& [k,v] : actor_jsons_with_unaltered_default_properties){
            v->Mark();
        }

        for(const auto& [k,v] : class_jsons){
            v->Mark();
        }
    }

    std::map<std::string, std::function<std::unique_ptr<Actor>(ufo::gc::JsonMap* _json)>> factory_map;

    virtual void Initialise();

    std::unique_ptr<Actor> JsonToActorTree(ufo::GarbageCollector* _gc, ufo::gc::JsonMap* _json);

    virtual void OnJsonToActorTree(Actor* _actor, ufo::gc::JsonMap* _json);

    virtual std::unique_ptr<Actor> FromJson(ufo::gc::JsonMap* _json);

    /*

       FromMinimalJson , for loading from a minimal json structure, like

        {
            "class_name":"..."
            "base_class_name":"..."
            "custom_editor_properties":{
                "..." : "...",
                ...
            }

        }

     */

    std::unique_ptr<Actor> FromJsonInGame(ufo::gc::JsonMap* _json);

    void JsonToActorTreeInGameComponentLoad(Actor* _actor, ufo::gc::JsonMap* _json);

    virtual ~GenericGenerator() = default;
};

}
