#pragma once
#include <exception>
#include <level.h>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include "file_node.h"
#include "tab.h"
#include <graphics.h>
#include <engine.h>
#include <map>
#include <functional>
#include "../ufo_garbage_collector/engine_memory.h"
#include "../src/actor.h"
#include <gc_json.h>
#include "../src/actor.h"
#include "../src/sprite.h"
#include "../src/camera.h"
#include "../ufo_maths/ufo_maths.h"

namespace UFOEngineStudio{

class Editor : public Level, public ufo::gc::Root{
public:
    std::string header_tool_parser = "parse_ufo_macros_v_alpha.py";

    std::string opened_directory_path = "";
    bool should_refresh_working_directory = false;

    bool refresh_entire_project = false;

    std::unique_ptr<FileNode> opened_directory = nullptr;

    std::vector<std::unique_ptr<Tab>> tabs;
    Tab* active_tab = nullptr;

    bool set_all_actors_properties_open_to_false = false;
    Editor();
    void OpenFolder(std::string _path);
    void Load();
    void ImportHeaderFileToProject(std::string _path);

    void OnUpdate(float _delta_time);

    void EditorUpdatePhase(float _delta_time){
        /*for(const auto& tab : tabs){
            tab->LevelUpdatePhase(_delta_time);
        }*/
    }

    void EditorDrawPhase(){
        /*for(const auto& tab : tabs){
            tab->LevelDrawPhase(engine->graphics.get());
        }*/
    }

    class AdvancedActorSpawner{
    public:
        std::string base;
        std::function<std::unique_ptr<Actor>(Editor* _editor, AdvancedActorSpawner* _this)> spawner_function;
        std::vector<std::unique_ptr<Actor::EditorProperty>> properties;

        AdvancedActorSpawner(
            std::function<std::unique_ptr<Actor>(Editor* _editor, AdvancedActorSpawner* _this)> _spawner_function,
            std::string _base = ""
        ) :
        spawner_function{_spawner_function},
        base{_base}
        {}

        std::unique_ptr<Actor> Spawn(Editor* _editor){
            auto act = spawner_function(_editor,this);

            for(const auto& property : properties){
                act->editor_properties.push_back(property->Copy());
            }

            return std::move(act);
        }
    };

    void PopulateSpawnableActorMapWithBaseObjects(){
        spawnable_actor_map.emplace("Actor",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<Actor>(Vector2f(0.0f, 0.0f));
            }))
        );

        spawnable_actor_map.emplace(
            "Sprite",
            std::move(std::make_unique<AdvancedActorSpawner>(
                [](Editor* _editor, AdvancedActorSpawner* _this){
                    return std::make_unique<Sprite>("placeholder_icon",
                        Vector2f(0.0f, 0.0f),
                        Vector2f(0.0f, 0.0f),
                        Vector2f(32.0f, 32.0f),
                        Vector2f(1.0f, 1.0f),
                        0.0f,
                        0
                    );
                }
            ))
        );

        spawnable_actor_map.emplace("Camera",
            std::move(std::make_unique<AdvancedActorSpawner>([](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<Camera>(Vector2f(0.0f, 0.0f));
            }))
        );
    }

    void ReloadSpawnableActorMap(){

        auto exported_actors_json = ufo::gc::JsonRead(&gc, opened_directory_path+"/structured_classes.json");
        for(const auto& j_class : exported_actors_json->map.at("contents")->AsArray()){
            auto class_ = j_class->AsMap().at("class")->AsMap();

            std::string inherits = "";
            if(class_.at("extends")->AsArray().size() > 0) inherits = class_.at("extends")->AsArray()[0]->AsString();

            Console::PrintLine("Inherits",inherits);

            auto act_spawner = std::make_unique<AdvancedActorSpawner>([&](Editor* _editor, AdvancedActorSpawner* _this){
                            return std::move(_editor->spawnable_actor_map.at(_this->base)->Spawn(_editor));
                        },
                        inherits
                    );

            for(const auto& macro : j_class->AsMap().at("macros")->AsArray()){
                std::string macro_name = macro->AsMap().at("name")->AsString();

                Console::PrintLine("Macro Name:",macro_name);
            }

            for(const auto& member : class_.at("members")->AsArray()){

                std::string name = member->AsArray()[1]->AsMap().at("name")->AsString();
                std::string value = member->AsArray()[1]->AsMap().at("variable_value")->AsString();
                std::string data_type = member->AsArray()[1]->AsMap().at("data_type")->AsString();
                std::string alias = name;

                for(const auto& macro : member->AsArray()[0]->AsArray()){
                    std::string macro_name = macro->AsMap().at("name")->AsString();

                    auto args = macro->AsMap().at("args")->AsArray();

                    Console::PrintLine("Member macro",macro_name);

                    if(macro_name == "ufo_alias") alias = args[0]->AsString();

                    if(macro_name == "ufo_int_slider"){

                        try{
                            act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyIntSlider>(
                                name,
                                alias,
                                std::stoi(value),
                                std::stoi(args[0]->AsString()),
                                std::stoi(args[1]->AsString())
                            ));
                        }
                        catch(const std::out_of_range& _error){
                            Console::PrintLine("Error in",class_.at("name")->AsString(), "ufo_int_slider takes 2 args", _error.what());
                        }
                        catch(const std::exception& _error){
                            Console::PrintLine("Error in",class_.at("name")->AsString(), _error.what());
                        }
                    }

                    if(macro_name == "ufo_variable"){

                        if(data_type == "int") act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyInt>(name,alias,std::stoi(value)));
                        if(data_type == "float") act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyFloat>(name,alias,std::stoi(value)));
                    }
                }
            }

            spawnable_actor_map.emplace(class_.at("name")->AsString(),
                std::move(
                    act_spawner
                )

            );

        }

    }

    void OnInvokeGarbageCollector(){
        gc.Collect();
    }

    std::map<std::string, std::unique_ptr<AdvancedActorSpawner>> spawnable_actor_map;

};

}
