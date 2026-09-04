#pragma once

#include <memory>
#include "actor.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/object.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{

class Engine;

class GenericGenerator : public ufo::gc::Root{
public:
    ufo::Engine* engine = nullptr;

    GenericGenerator(){
        garbage_collected_object_name = "GC_GenericGenerator";
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
    std::map<std::string, std::function<std::unique_ptr<Actor>(Vector2f)>> runtime_factory_map;

    virtual void Initialise(ufo::Engine* _engine);

    std::unique_ptr<Actor> JsonToActorTree(ufo::GarbageCollector* _gc, ufo::gc::JsonMap* _json);
    //Not done yet
    std::unique_ptr<Actor> SpawnAtRuntime(const std::string& _class_name, Vector2f _local_position);

    // Overridden in generated.h
    virtual std::unique_ptr<Actor> FromJson(ufo::gc::JsonMap* _json);

    std::unique_ptr<Actor> FromJsonInGame(ufo::gc::JsonMap* _json);

    virtual ~GenericGenerator() = default;
};

}
