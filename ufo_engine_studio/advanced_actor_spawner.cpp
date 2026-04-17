#include <memory>
#include <functional>
#include <string>
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/object.h"
#include "../src/actor.h"
#include "editor.h"
#include "advanced_actor_spawner.h"

namespace UFOEngineStudio{

AdvancedActorSpawner::AdvancedActorSpawner(
    std::function<std::unique_ptr<ufo::Actor>(Editor* _editor, AdvancedActorSpawner* _this)> _spawner_function,
    std::string _base,
    std::string _class_name
) :
spawner_function{_spawner_function},
base{_base},
class_name{_class_name}
{
    MakeMarkable(&actor_as_json);
}

void AdvancedActorSpawner::OnMark() {
    //Console::PrintLine("AdvancedActorSpawner marked!");
}

std::unique_ptr<ufo::Actor> AdvancedActorSpawner::Spawn(Editor* _editor){
    auto act = spawner_function(_editor,this);

    if(actor_config_path != ""){
        ufo::gc::JsonMap* actor_config = ufo::gc::JsonRead(&(_editor->gc), _editor->opened_directory_path+"/"+actor_config_path);

        for(const auto& j_actor : actor_config->map.at("actors")->AsArray()){
            if(j_actor->AsMap().at("name")->AsString() == "Main"){
                act->editor_name = "@" + act->class_name;
                auto actor_from_file = _editor->engine->actor_generator->JsonToActorTree(&(_editor->gc),dynamic_cast<ufo::gc::JsonMap*>(j_actor));

                //Need to somehow know if these actors are imported
                //for(auto& actor : actor_from_file->new_actor_queue) actor->DeclareImportedRecursive();

                actor_from_file->import_mode = ufo::Actor::ImportModes::WRAPPED;

                std::string base_class_of_actor_config = j_actor->AsMap().at("base_class_name")->AsString();
                if(base_class_of_actor_config != act->base_class_name){
                    Console::PrintLine("[UFO-Engine Studio] AdvancedActorSpawner base of this", act->class_name, "does not match base of actor_config",
                        actor_config, "Baseclass of actor config:", base_class_of_actor_config, "Baseclass of this", act->base_class_name
                    );
                }
                else{
                    act = std::move(actor_from_file);
                }
            }
        }
    }


    for(const auto& property : custom_properties){
        act->editor_properties.push_back(property->Copy());
    }

    act->class_name = class_name;
    act->editor_name = act->class_name+std::to_string(_editor->actor_count_for_naming_purposes++);

    return std::move(act);
}

}
