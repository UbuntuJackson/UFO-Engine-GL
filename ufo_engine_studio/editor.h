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
#include "../tilemap/tile_map.h"
#include "../src/text.h"
#include "../src/widget.h"
#include "../src/animation.h"
#include "../src/button.h"
#include "../src/platformer_rectangle_collision.h"
#include "../src/collision_grid.h"

namespace UFOEngineStudio{

class Editor : public Level, public ufo::gc::Root{
public:

    bool will_compile_game = false;
    bool will_run_game = false;
    bool v_sync = true;

    int game_width = 1600;
    int game_height = 900;

    bool multi_player = false;

    bool project_settings_open = false;

    //The path to the UFO-Engine Header Tool & Boilerplate generator
    std::string header_tool_parser = "ufo_engine_header_tool.py";

    //The current working directory for the projekt. Not for this program.
    std::string opened_directory_path = "";

    std::string currently_selected_actor_type = "";

    bool refresh_entire_project = false;

    std::unique_ptr<FileNode> opened_directory = nullptr;

    std::vector<std::unique_ptr<Tab>> tabs;
    Tab* active_tab = nullptr;

    bool set_all_actors_properties_open_to_false = false;
    Editor();
    void OpenFolder(std::string _path);
    void RefreshFolder();
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
        std::string actor_config_path = "";
        std::string category = "UFO-Engine";
        std::string comment = "";

        std::string base;
        std::string class_name = "";
        std::function<std::unique_ptr<Actor>(Editor* _editor, AdvancedActorSpawner* _this)> spawner_function;
        std::vector<std::unique_ptr<Actor::EditorProperty>> properties;

        AdvancedActorSpawner(
            std::function<std::unique_ptr<Actor>(Editor* _editor, AdvancedActorSpawner* _this)> _spawner_function,
            std::string _base = "",
            std::string _class_name = ""
        ) :
        spawner_function{_spawner_function},
        base{_base},
        class_name{_class_name}
        {}

        std::unique_ptr<Actor> Spawn(Editor* _editor){
            auto act = spawner_function(_editor,this);

            if(actor_config_path != ""){
                ufo::gc::JsonMap* actor_config = ufo::gc::JsonRead(&(_editor->gc), _editor->opened_directory_path+"/"+actor_config_path);

                for(const auto& j_actor : actor_config->map.at("actors")->AsArray()){
                    if(j_actor->AsMap().at("name")->AsString() == "Main"){
                        act->editor_name = "@" + act->class_name;
                        auto actor_from_file = _editor->engine->actor_generator->JsonToActorTree(&(_editor->gc),dynamic_cast<ufo::gc::JsonMap*>(j_actor));

                        //Need to somehow know if these actors are imported
                        //for(auto& actor : actor_from_file->new_actor_queue) actor->DeclareImportedRecursive();

                        actor_from_file->import_mode = Actor::ImportModes::WRAPPED;

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


            for(const auto& property : properties){
                act->editor_properties.push_back(property->Copy());
            }

            act->class_name = class_name;
            act->editor_name = act->class_name+std::to_string(_editor->actor_count_for_naming_purposes++);

            return std::move(act);
        }
    };

    int actor_count_for_naming_purposes = 0;

    void PopulateSpawnableActorMapWithBaseObjects(){
        spawnable_actor_map.emplace("Actor",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<Actor>(Vector2f(0.0f, 0.0f));
            }, "Actor", "Actor"))
        );

        spawnable_actor_map.emplace("ufo::CollisionGrid",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::CollisionGrid>(Vector2f(0.0f, 0.0f));
            }, "ufo::CollisionGrid", "ufo::CollisionGrid"))
        );

        spawnable_actor_map.emplace("ufo::PlatformerRectangleCollision",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::PlatformerRectangleCollision>(Vector2f(0.0f, 0.0f));
            }, "ufo::PlatformerRectangleCollision", "ufo::PlatformerRectangleCollision"))
        );

        spawnable_actor_map.emplace("TileMap",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<TileMap>(Vector2f(0.0f, 0.0f));
            }, "TileMap", "TileMap"))
        );

        spawnable_actor_map.emplace("Level",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<Level>();
            }, "Level", "Level"))
        );

        spawnable_actor_map.emplace("Widget",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::Widget>(Vector2f(0.0f, 0.0f));
            }, "Widget", "Widget"))
        );

        spawnable_actor_map.emplace("Text",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::Text>(Vector2f(0.0f, 0.0f));
            }, "Text", "Text"))
        );

        spawnable_actor_map.emplace("Button",std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::Button>(Vector2f(0.0f, 0.0f));
            }, "Button", "Button"))
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
                },
                "Sprite",
                "Sprite"
            ))
        );

        spawnable_actor_map.emplace(
            "Animation",
            std::move(std::make_unique<AdvancedActorSpawner>(
                [](Editor* _editor, AdvancedActorSpawner* _this){
                    return std::make_unique<Animation>(
                        Vector2f(0.0f, 0.0f)
                    );
                },
                "Animation",
                "Animation"
            ))
        );

        spawnable_actor_map.emplace("Camera",
            std::move(std::make_unique<AdvancedActorSpawner>([](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<Camera>(Vector2f(0.0f, 0.0f));
            },
            "Camera", "Camera"))
        );
    }

    void ReloadSpawnableActorMap(){

        auto exported_actors_json = ufo::gc::JsonRead(&gc, opened_directory_path+"/structured_classes.json");
        for(const auto& j_class : exported_actors_json->map.at("contents")->AsArray()){
            auto class_ = j_class->AsMap().at("class")->AsMap();

            std::string inherits = "";
            if(class_.at("extends")->AsArray().size() > 0) inherits = class_.at("extends")->AsArray()[0]->AsString();

            auto act_spawner = std::make_unique<AdvancedActorSpawner>([&](Editor* _editor, AdvancedActorSpawner* _this){
                            if(_editor->spawnable_actor_map.count(_this->base)){
                                auto instance = _editor->spawnable_actor_map.at(_this->base)->Spawn(_editor);

                                return std::move(instance);
                            }

                            Console::PrintLine("[UFO-Engine Studio] Editor::ReloadSpawnableActorMap: Error, could not find spawner of base-type",
                                _this->base,
                                "and type", _this->class_name);

                            return _editor->spawnable_actor_map.at("Actor")->Spawn(_editor);
                        },
                        inherits,
                        class_.at("name")->AsString()
                    );

            for(const auto& macro : j_class->AsMap().at("macros")->AsArray()){
                if(macro->AsMap().at("name")->AsString() == "ufo_actor_config"){

                        auto arr = macro->AsMap().at("args")->AsArray();
                        if(arr.size() == 1){
                            if(std::filesystem::exists(opened_directory_path+"/"+arr[0]->AsString())){
                                act_spawner->actor_config_path = arr[0]->AsString();
                            }
                            else Console::PrintLine("[UFO-Engine Studio] Faulty actor_config path for class", class_.at("name")->AsString(), opened_directory_path+"/"+arr[0]->AsString());
                        }
                }
                if(macro->AsMap().at("name")->AsString() == "ufo_category"){

                        auto arr = macro->AsMap().at("args")->AsArray();
                        if(arr.size() == 1){
                            act_spawner->category = arr[0]->AsString();

                        }
                }

                if(macro->AsMap().at("name")->AsString() == "ufo_comment"){

                        auto arr = macro->AsMap().at("args")->AsArray();
                        if(arr.size() == 1){
                            act_spawner->comment = arr[0]->AsString();

                        }
                }
            }

            for(const auto& member : class_.at("members")->AsArray()){

                std::string name = "<Faulty Name>";
                std::string value = "<Faulty Value>";
                std::string data_type = "<Faulty DataType>";

                //This function is full of potential conversion errors due to faulty syntax. At least try handle them.

                if(member->AsArray()[1]->AsMap().count("name")) name = member->AsArray()[1]->AsMap().at("name")->AsString();
                else{
                    Console::PrintLine("[UFO-Engine Studio] Editor::ReloadSpawnableActorMap: Faulty variable name");
                    continue;
                }

                if(member->AsArray()[1]->AsMap().count("variable_value")) value = member->AsArray()[1]->AsMap().at("variable_value")->AsString();
                else{
                    Console::PrintLine("[UFO-Engine Studio] Editor::ReloadSpawnableActorMap: Faulty variable value");
                    continue;
                }

                if(member->AsArray()[1]->AsMap().count("data_type")) data_type = member->AsArray()[1]->AsMap().at("data_type")->AsString();
                else{
                    Console::PrintLine("[UFO-Engine Studio] Editor::ReloadSpawnableActorMap: Faulty variable datatype");
                    continue;
                }

                std::string alias = name;

                for(const auto& macro : member->AsArray()[0]->AsArray()){
                    std::string macro_name = macro->AsMap().at("name")->AsString();

                    auto args = macro->AsMap().at("args")->AsArray();

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

                    if(macro_name == "ufo_float_slider"){

                        try{
                            act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyFloatSlider>(
                                name,
                                alias,
                                std::stof(value),
                                std::stof(args[0]->AsString()),
                                std::stof(args[1]->AsString()),
                                std::stof(args[2]->AsString())
                            ));
                        }
                        catch(const std::out_of_range& _error){
                            Console::PrintLine("Error in",class_.at("name")->AsString(), "ufo_float_slider takes 3 args", _error.what());
                        }
                        catch(const std::exception& _error){
                            Console::PrintLine("Error in",class_.at("name")->AsString(), _error.what());
                        }
                    }

                    if(macro_name == "ufo_variable"){

                        if(data_type == "int") act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyInt>(name,alias,std::stoi(value)));
                        if(data_type == "float") act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyFloat>(name,alias,std::stoi(value)));
                        if(data_type == "bool"){
                            act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyCheckBox>(name,alias,(bool)std::stoi(value)));
                        }
                        if(data_type == "std::string"){
                            act_spawner->properties.push_back(std::make_unique<Actor::EditorPropertyString>(name,alias,value));
                        }
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

    std::map<std::string, std::unique_ptr<AdvancedActorSpawner>> spawnable_actor_map;

};

void BuildAndRunProgram(const std::string& _build_directory, const std::string& _opened_directory_path);
void DebugGame(const std::string& _build_directory, const std::string& _opened_directory_path);
void RunGame(const std::string& _build_directory, const std::string& _opened_directory_path);

}
