#include <exception>
#include <level.h>
#include <string>
#include <camera.h>
#include <filesystem>
#include "console.h"
#include "file_node.h"
#include "../imgui/imgui.h"
#include "../ufo_engine_studio/dock_utils.h"
#include "file_node.h"
#include "tab.h"
#include "editor.h"
#include <cstdlib>
#include "../src/engine.h"
#include "level_editor_tab.h"
#include "text_editor_tab.h"
#include "file_dialogue.h"
#include "../file/file_utils.h"
#include "../imgui/imgui_internal.h"
#include "../ufo_maths/math_parser.h"

namespace UFOEngineStudio{

Editor::Editor(){
    name = "GC_Editor";
}

void Editor::ResetUFOEngineStudio(){
    engine->asset_manager.SaveAssets();
    engine->asset_manager.shaders.clear();
    engine->asset_manager.textures.clear();

    engine->asset_manager.Initialise_UFOEngineStudio(this,engine);
}

void Editor::OpenFolder(std::string _path){
    project_settings = ProjectSettings{};

    spawnable_actor_map.clear();

    opened_directory = FileNode::ParseFolder(_path);
    opened_directory->file_name = "";

    opened_directory_path = _path;

    ResetUFOEngineStudio();

    tabs.clear();
    active_tab = nullptr;

    if(ufo::FileSystem::FileExists(opened_directory_path+"/settings.json")){
        auto j_settings = ufo::gc::JsonRead(&gc, opened_directory_path+"/settings.json");
        try{
            if(!j_settings->map.count("vsync")) j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(0.0f);
            if(!j_settings->map.count("multi_player")) j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(0.0f);
            if(!j_settings->map.count("game_width")) j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(1600.0f);
            if(!j_settings->map.count("game_height")) j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(900.0f);
            if(!j_settings->map.count("game_window_title")) j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>("");

             project_settings.v_sync = (bool)j_settings->map["vsync"]->AsFloat();
             project_settings.multi_player = (bool)j_settings->map["multi_player"]->AsFloat();
             project_settings.game_width = (int)j_settings->map["game_width"]->AsFloat();
             project_settings.game_height = (int)j_settings->map["game_height"]->AsFloat();
             project_settings.game_window_title = j_settings->map["game_window_title"]->AsString();
        }catch(const std::exception& _error){
            Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
        }
        j_settings->Write(opened_directory_path+"/settings.json");
    }
    else{
        auto j_settings = gc.New<ufo::gc::JsonMap>();
        try{
            j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.v_sync));
            j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_width));
            j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_height));
            j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.multi_player));
            j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>(project_settings.game_window_title);
        }catch(const std::exception& _error){
            Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
        }
        j_settings->Write(opened_directory_path+"/settings.json");
    }

    refresh_entire_project = true;
}

void Editor::RefreshFolder(){

    opened_directory = FileNode::ParseFolder(opened_directory_path);
    opened_directory->file_name = "";

}

void
Editor::Load(){
    OpenFolder("/home/uj/Documents/C++/sta_replica");

}

void Editor::ImportHeaderFileToProject(std::string _path){

}

void Editor::OnUpdate(float _delta_time){

    ImGuiWindowFlags im_gui_window_flags = ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_MenuBar;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::Begin("DemoDockspaceForGodsSake", nullptr, im_gui_window_flags);

    ImGuiID dock_space_id = ImGui::GetID("DemoDockspaceForGodsSake");

    UFOEngineStudio::ImGuiDockSpaceSplit(dock_space_id, viewport->Size, "File Tree", "TabBarWindow", UFOEngineStudio::SplitDirections::HORIZONTAL);

    ImGui::DockSpace(dock_space_id, ImVec2(0.0f,0.0f), ImGuiDockNodeFlags_NoTabBar);

    ImGui::End();



    ImGui::Begin("File Tree");

    opened_directory->Update(0, nullptr, "", this);

    opened_directory->AddFileNodesRecursive();

    ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open Folder")){
                SDL_ShowOpenFolderDialog(&UFOEngineStudio::OnOpenFolder, this, engine->window, "/home", false);
            }

            if(ImGui::MenuItem("Save")){
                if(active_tab){
                    active_tab->OnSave(this);
                    refresh_entire_project = true;
                }
            }

            if (ImGui::BeginMenu("New File"))
            {
                if(ImGui::MenuItem("Level (.ason)")){
                    auto tab = std::make_unique<LevelEditorTab>(engine,this);
                    tab->Initialise();
                    tabs.push_back(std::move(tab));
                }
                if(ImGui::MenuItem("Textfile (.txt)")){
                    tabs.push_back(std::make_unique<TextEditorTab>("","",this));
                }
                /*if(ImGui::MenuItem("Actor (ufo.h)")){
                    tabs.push_back(std::make_unique<TextEditorTab>("","",this));
                    }*/

                ImGui::EndMenu();
            }


            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Project")){

            if(ImGui::MenuItem("Reload Project")){
                refresh_entire_project = true;
            }

            if(ImGui::MenuItem("Compile Game")){
                refresh_entire_project = true;
                will_compile_game = true;
            }

            if(ImGui::MenuItem("Debug Game")){
                const std::string build_directory = opened_directory_path+"/build";
                std::thread t(&DebugGame, build_directory, opened_directory_path);
                t.detach();
            }

            if(ImGui::MenuItem("Run Game")){
                const std::string build_directory = opened_directory_path+"/build";
                std::thread t(&RunGame, build_directory, opened_directory_path);
                t.detach();
            }

            if(ImGui::MenuItem("Settings")){
                project_settings_open = true;
            }

            ImGui::EndMenu();
        }
        if(ImGui::BeginMenu("Tools")){
            if(ImGui::MenuItem("Calculator")){
                view_calculator = true;
            }

            ImGui::EndMenu();
        }
        if(ImGui::MenuItem("Undo")){
            if(active_tab){
                LevelEditorTab* tab = dynamic_cast<LevelEditorTab*>(active_tab);
                tab->this_level->Undo();
            }
        }
        if(ImGui::MenuItem("Redo")){
            if(active_tab){
                LevelEditorTab* tab = dynamic_cast<LevelEditorTab*>(active_tab);
                tab->this_level->Redo();
            }
        }

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin("TabBarWindow");

    ImGui::BeginTabBar("TabBar");

    for(const auto& tab : tabs){
        tab->Update(this, _delta_time);
    }

    ImGui::EndTabBar();

    ImGui::End();

    if(view_calculator){
        ImGui::Begin("Calculator", &view_calculator);

        if(ImGui::InputText("Expression:", &calculator_expression, ImGuiInputTextFlags_EnterReturnsTrue)){
            float result = ufo::Maths::ParseExpression(calculator_expression);
            Console::PrintLine("Result:",result);
        }

        ImGui::End();
    }

    if(project_settings_open){
        ImGui::Begin("Game Settings", &project_settings_open);

        ImGui::Checkbox("Vsync###EditorVsync On", &project_settings.v_sync);
        ImGui::Checkbox("MultiPlayer###Multiplayer On", &project_settings.multi_player);

        ImGui::InputInt("Window Width", &project_settings.game_width);
        ImGui::InputInt("Window Height", &project_settings.game_height);

        ImGui::InputText("Game Window Title:", &project_settings.game_window_title);

        if(ImGui::Button("Apply & Save")){
            if(ufo::FileSystem::FileExists(opened_directory_path+"/settings.json")){
                auto j_settings = ufo::gc::JsonRead(&gc, opened_directory_path+"/settings.json");
                try{
                    if(!j_settings->map.count("vsync")) j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(0.0f);
                    if(!j_settings->map.count("multi_player")) j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(0.0f);
                    if(!j_settings->map.count("game_width")) j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(1600.0f);
                    if(!j_settings->map.count("game_height")) j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(900.0f);
                    if(!j_settings->map.count("game_window_title")) j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>("");

                    j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.v_sync));
                    j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_width));
                    j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_height));
                    j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.multi_player));
                    j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>(project_settings.game_window_title);
                }catch(const std::exception& _error){
                    Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
                }
                j_settings->Write(opened_directory_path+"/settings.json");
            }
            else{
                auto j_settings = gc.New<ufo::gc::JsonMap>();
                try{
                    j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.v_sync));
                    j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_width));
                    j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_height));
                    j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.multi_player));
                    j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>(project_settings.game_window_title);
                }catch(const std::exception& _error){
                    Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
                }
                j_settings->Write(opened_directory_path+"/settings.json");
            }
            project_settings_open = false;
        }

        ImGui::SameLine();

        if(ImGui::Button("Cancel")){
            project_settings_open = false;
        }

        ImGui::End();
    }

    //UFOEngineStudio::ImGuiDockSpaceSplit(dock_space_id, viewport->Size, "File Tree", "TabBarWindow", UFOEngineStudio::SplitDirections::HORIZONTAL);

    if(refresh_entire_project){

        //Here I'm forcing an update on the new actor queue to make sure there are actors in level are loaded before
        // making potential modifications to them, like adding or removing ufo-variables.
        AddNewActors();

        spawnable_actor_map.clear();
        std::system(std::string(std::string("cd ../UFO-Engine/header_tool && python3 "+header_tool_parser + " ")+std::string("\"")+opened_directory_path+std::string("\"")).c_str());
        //std::system(std::string(std::string("cd ../UFO-Engine/header_tool && python3 generate_actor_spawner_functions.py" + std::string(" "))+std::string("\"")+opened_directory_path+std::string("\"")).c_str());

        PopulateSpawnableActorMapWithBaseObjects();
        ReloadSpawnableActorMap();
        for(const auto& tab : tabs){
            tab->Refresh();
        }
        engine->asset_manager.SaveAssets();
        Console::PrintLine("Refreshed entire project");

        //Refresh entire file tree.
        RefreshFolder();

        refresh_entire_project = false;
    }

    if(will_compile_game){
        //std::system("cd ../UFO-Engine/header_tool && chmod +x compile_game.sh && ./compile_game.sh");

        const std::string build_directory = opened_directory_path+"/build";
        std::thread t(&BuildAndRunProgram, build_directory, opened_directory_path);
        t.detach();
        will_compile_game = false;
    }

    gc.Collect();
}

void Editor::PopulateSpawnableActorMapWithBaseObjects(){
    spawnable_actor_map.emplace("ufo::Actor",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Actor>(Vector2f(0.0f, 0.0f));
        }, "ufo::Actor", "ufo::Actor"))
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

    spawnable_actor_map.emplace("ufo::TileMap",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::TileMap>(Vector2f(0.0f, 0.0f));
        }, "ufo::TileMap", "ufo::TileMap"))
    );

    spawnable_actor_map.emplace("ufo::Level",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Level>();
        }, "ufo::Level", "ufo::Level"))
    );

    spawnable_actor_map.emplace("ufo::Widget",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Widget>(Vector2f(0.0f, 0.0f));
        }, "ufo::Widget", "ufo::Widget"))
    );

    spawnable_actor_map.emplace("ufo::Text",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Text>(Vector2f(0.0f, 0.0f));
        }, "ufo::Text", "ufo::Text"))
    );

    spawnable_actor_map.emplace("ufo::Button",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Button>(Vector2f(0.0f, 0.0f));
        }, "ufo::Button", "ufo::Button"))
    );

    spawnable_actor_map.emplace(
        "ufo::Sprite",
        std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::Sprite>("placeholder_icon",
                    Vector2f(0.0f, 0.0f),
                    Vector2f(0.0f, 0.0f),
                    Vector2f(32.0f, 32.0f),
                    Vector2f(1.0f, 1.0f),
                    0.0f,
                    0
                );
            },
            "ufo::Sprite",
            "ufo::Sprite"
        ))
    );

    spawnable_actor_map.emplace(
        "ufo::BackgroundSprite",
        std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::BackgroundSprite>(
                    Vector2f(0.0f, 0.0f)
                );
            },
            "ufo::BackgroundSprite",
            "ufo::BackgroundSprite"
        ))
    );

    spawnable_actor_map.emplace(
        "ufo::Animation",
        std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::Animation>(
                    Vector2f(0.0f, 0.0f)
                );
            },
            "ufo::Animation",
            "ufo::Animation"
        ))
    );

    spawnable_actor_map.emplace("ufo::Camera",
        std::move(std::make_unique<AdvancedActorSpawner>([](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Camera>(Vector2f(0.0f, 0.0f));
        },
        "ufo::Camera", "ufo::Camera"))
    );

    spawnable_actor_map.emplace("ufo::RectangularArea",
        std::move(std::make_unique<AdvancedActorSpawner>([](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::RectangularArea>(Vector2f(0.0f, 0.0f));
        },
        "ufo::RectangularArea", "ufo::RectangularArea"))
    );
}

void Editor::ReloadSpawnableActorMap(){

    auto exported_actors_json = ufo::gc::JsonRead(&gc, opened_directory_path+"/structured_classes.json");

    if(exported_actors_json->IsNull()){
        Console::PrintLine("[UFO-Engine Studio] Warning: Could not find file with exported actors",opened_directory_path+"/structured_classes.json");
        return;
    }

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

                        return _editor->spawnable_actor_map.at("ufo::Actor")->Spawn(_editor);
                    },
                    inherits,
                    class_.at("name")->AsString()
                );

        bool hide_from_editor = false;

        for(const auto& macro : j_class->AsMap().at("macros")->AsArray()){
            if(macro->AsMap().at("name")->AsString() == "ufo_hide_from_editor"){
                hide_from_editor = true;
                break;
            }

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

        if(hide_from_editor) continue;

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
                        act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyIntSlider>(
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
                        act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyFloatSlider>(
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

                //Small glaring issue, but if you put ufo_variable before ufo_alias the property will be pushed without the alias.

                if(macro_name == "ufo_variable"){

                    if(data_type == "int") act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyInt>(name,alias,std::stoi(value)));
                    if(data_type == "float") act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyFloat>(name,alias,std::stoi(value)));
                    if(data_type == "bool"){
                        act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyCheckBox>(name,alias,(bool)std::stoi(value)));
                    }
                    if(data_type == "std::string"){
                        act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyString>(name,alias,value));
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

void Editor::OnMark() {
    for(const auto& [k,v] : spawnable_actor_map){
        v->Mark();
    }
}

void BuildAndRunProgram(const std::string& _build_directory, const std::string& _opened_directory_path){
    try{
        const std::string cmake_file_path = _opened_directory_path+"/CMakeLists.txt";
        if(!ufo::FileSystem::FileExists(cmake_file_path)){
            const std::string cmake_file = ufo::FileSystem::Read("../UFO-Engine/project_templates/CMakeLists.txt");
            ufo::FileSystem::Write(cmake_file_path, cmake_file);
        }
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    try{
        const std::string main_file_path = _opened_directory_path+"/main.cpp";

        if(!ufo::FileSystem::FileExists(main_file_path)){
            std::string main_file = ufo::FileSystem::Read("../UFO-Engine/project_templates/main.cpp");
            ufo::FileSystem::Write(main_file_path, main_file);
        }
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    if(ufo::FileSystem::FileExists(_opened_directory_path+"/UFO-Engine-GL")){
        Console::PrintLine("[UFO-Engine Studio] Error, please rename cloned repository to UFO-Engine");
        return;
    }

    if(!ufo::FileSystem::FileExists(_opened_directory_path+"/UFO-Engine")){
        Console::PrintLine("[UFO-Engine Studio] Could not find folder "+_opened_directory_path+"/UFO-Engine");
        return;
    }

    if(!std::filesystem::exists(_build_directory.c_str())){
        std::filesystem::create_directory(_build_directory.c_str());
    }

    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"cmake .. -DCMAKE_CXX_FLAGS=\"-ggdb\" && make -j8 && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Project Process Success?", success);
}

void DebugGame(const std::string& _build_directory, const std::string& _opened_directory_path){
    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"gdb OUT && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Game Run Success?", success);
}

void RunGame(const std::string& _build_directory, const std::string& _opened_directory_path){
    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"./OUT && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Game Run Success?", success);
}

}
