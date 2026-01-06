#pragma once
#include "../ufo_garbage_collector/engine_memory.h"
#include "../ufo_garbage_collector/gc_json.h"
#include <memory>
#include "generic_generator.h"

namespace ufo{

class ActorComponentLoader : public ufo::gc::Root{
public:
    void Load(ufo::GenericGenerator* _actor_generator, Actor* _parent_actor){

        Console::PrintLine("[UFO-Engine] Fetching additional components for instance of type",_parent_actor->class_name);

        std::string actor_config_path = "";

        bool found_class = false;

        ufo::gc::JsonMap* structured_classes_json = ufo::gc::JsonRead(&gc, "../structured_classes.json");
        auto classes = structured_classes_json->map.at("contents")->AsArray();
        for(const auto& cl : classes){
            auto macros = cl->AsMap().at("macros")->AsArray();
            found_class = cl->AsMap().at("class")->AsMap().at("name")->AsString() == _parent_actor->class_name;
            if(!found_class) continue;

            for(const auto& macro : macros){
                if(macro->AsMap().at("name")->AsString() == "ufo_actor_config"){
                    auto args = macro->AsMap().at("args")->AsArray();

                    if(args.size() == 1){
                        actor_config_path = args[0]->AsString();
                        Console::PrintLine("Found actor_config_path argument",actor_config_path);
                    }
                    else{
                        Console::PrintLine("[UFO-Engine] ActorComponentLoader: ufo_actor_config has too few arguments", _parent_actor->class_name);
                    }
                }
            }
            if(found_class) break;
        }

        if(!found_class){
            Console::PrintLine("[UFO-Engine] Error: Could not fine ufo_actor_config file for class", _parent_actor->class_name);
            return;
        }

        //Now using the config path

        ufo::gc::JsonMap* actor_config = ufo::gc::JsonRead(&gc, "../"+actor_config_path);

        for(const auto& j_actor : actor_config->map.at("actors")->AsArray()){
            if(j_actor->AsMap().at("name")->AsString() == "Main"){
                auto actor_from_file = _actor_generator->JsonToActorTree(&(gc),dynamic_cast<ufo::gc::JsonMap*>(j_actor));

                //Need to somehow know if these actors are imported
                //for(auto& actor : actor_from_file->new_actor_queue) actor->DeclareImportedRecursive();

                std::string base_class_of_actor_config = j_actor->AsMap().at("base_class_name")->AsString();
                if(base_class_of_actor_config != _parent_actor->base_class_name){
                    Console::PrintLine("[UFO-Engine Studio] AdvancedActorSpawner base of this", _parent_actor->class_name, "does not match base of actor_config",
                        actor_config, "Baseclass of actor config:", base_class_of_actor_config, "Baseclass of this", _parent_actor->base_class_name
                    );
                }
                else{

                }

                for(auto&& actor : actor_from_file->new_actor_queue){
                    _parent_actor->AddActorUniquePtr(std::move(actor));
                }

            }
        }
    }

};

}
