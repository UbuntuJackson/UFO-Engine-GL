#pragma once
#include <memory>
#include <functional>
#include <string>
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/object.h"
#include "../src/actor.h"


namespace UFOEngineStudio{

class Editor;

class AdvancedActorSpawner : public ufo::gc::Object{
public:

    std::string actor_config_path = "";
    std::string category = "";
    std::string comment = "";

    std::string base;
    std::string class_name = "";
    std::function<std::unique_ptr<ufo::Actor>(Editor* _editor, AdvancedActorSpawner* _this)> spawner_function;
    std::vector<std::unique_ptr<ufo::EditorProperty>> custom_properties;
    ufo::gc::JsonMap* actor_as_json = nullptr;

    AdvancedActorSpawner(
        std::function<std::unique_ptr<ufo::Actor>(Editor* _editor, AdvancedActorSpawner* _this)> _spawner_function,
        std::string _base = "",
        std::string _class_name = "",
        std::string _category = "Miscellaneous"
    );

    void OnMark();

    std::unique_ptr<ufo::Actor> Spawn(Editor* _editor);
};

}
