#include <memory>
#include <functional>
#include <string>
#include "../ufo_garbage_collector/gc_json.h"
#include "../src/actor.h"
#include "editor.h"
#include "advanced_actor_spawner.h"
#include "error_dialogue.h"

namespace UFOEngineStudio{

AdvancedActorSpawner::AdvancedActorSpawner(
    std::function<std::unique_ptr<ufo::Actor>(Editor* _editor, AdvancedActorSpawner* _this)> _spawner_function,
    std::string _base,
    std::string _class_name,
    std::string _category
) :
spawner_function{_spawner_function},
base{_base},
class_name{_class_name},
category{_category}
{
    MakeMarkable(&actor_as_json);
}

void AdvancedActorSpawner::OnMark() {
    //Console::PrintLine("AdvancedActorSpawner marked!");
}

std::unique_ptr<ufo::Actor> AdvancedActorSpawner::Spawn(Editor* _editor){
    auto act = spawner_function(_editor,this);

    if(_editor->engine->actor_generator->actor_jsons_with_unaltered_default_properties.count(class_name)){
        ufo::gc::JsonMap* j_actor = _editor->engine->actor_generator->actor_jsons_with_unaltered_default_properties.at(class_name);

        auto actor_from_file = _editor->engine->actor_generator->JsonToActorTree(&(_editor->gc),dynamic_cast<ufo::gc::JsonMap*>(j_actor));

        actor_from_file->import_mode = ufo::Actor::ImportModes::CUSTOM_CLASS;
        act = std::move(actor_from_file);

    }

    for(const auto& property : custom_properties){
        act->editor_properties.push_back(property->Copy());
    }

    act->class_name = class_name;
    act->editor_name = act->class_name+std::to_string(_editor->actor_count_for_naming_purposes++);

    if(!act->is_editor_hit_box_unique_per_instance && _editor->engine->actor_generator->actor_jsons_with_unaltered_default_properties.count(act->class_name)){
        ufo::gc::JsonMap* class_json = _editor->engine->actor_generator->actor_jsons_with_unaltered_default_properties.at(act->class_name);

        //Some actor types don't have a default editor_hitbox
        if(class_json->map.count("editor_hitbox")){
            auto j_editor_hitbox = class_json->map.at("editor_hitbox")->AsMap();
            act->editor_hitbox = ufo::Rectangle(
                Vector2f(j_editor_hitbox.at("x")->AsFloat(),
                         j_editor_hitbox.at("y")->AsFloat()),
                Vector2f(j_editor_hitbox.at("width")->AsFloat(),
                         j_editor_hitbox.at("height")->AsFloat()
                ));
        }
    }

    return std::move(act);
}

}
