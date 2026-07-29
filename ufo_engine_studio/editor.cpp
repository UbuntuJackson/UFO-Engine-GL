#include "../src/ufo_macros.h"
#include "animation_cluster.h"
#include <stdexcept>
#define SDL_MAIN_HANDLED
#include <exception>
#include <level.h>
#include <string>
#include <camera.h>
#include <filesystem>
#include "SDL3/SDL_dialog.h"
#include "console.h"
#include "error_dialogue.h"
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
#include "../utils/file_utils.h"
#include "../imgui/imgui_internal.h"
#include "../ufo_maths/math_parser.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../src/animation_cluster.h"

namespace UFOEngineStudio{

Editor::Editor(){
    name = "GC_Editor";
}

void Editor::ResetUFOEngineStudio(){
    engine->asset_manager.SaveAssets();

    //Commented out for now, caused issues
    //for(auto& shader : engine->asset_manager.shaders) shader.second.Delete();
    engine->asset_manager.shaders.clear();

    engine->asset_manager.textures.clear();

    engine->graphics->InitialiseRenderData(engine);

    engine->asset_manager.Initialise_UFOEngineStudio(this,engine);
}

void Editor::OpenFolder(std::string _path){
    project_settings = ProjectSettings{};

    spawnable_actor_map.clear();

    opened_directory = FileNode::ParseFolder(_path);
    opened_directory->file_name = "";

    opened_directory_path = _path;

    recently_opened.insert(_path);


    ufo::gc::JsonMap* json_config = gc.New<ufo::gc::JsonMap>();
    ufo::gc::JsonArray* json_array = gc.New<ufo::gc::JsonArray>();
    for(const std::string& recently_opened_path : recently_opened){
        json_array->array.push_back(gc.New<ufo::gc::JsonString>(recently_opened_path));
    }
    json_config->map.emplace("recently_opened", json_array);
    json_config->Write("../editor_config.json");

    tabs.clear();
    active_tab = nullptr;

    if(ufo::FileSystem::FileExists(opened_directory_path+"/settings.json")){
        auto j_settings = ufo::gc::JsonRead(&gc, opened_directory_path+"/settings.json");
        try{
            ProjectSettings default_settings;

            if(!j_settings->map.count("vsync")) j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(default_settings.v_sync);
            if(!j_settings->map.count("multi_player")) j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(default_settings.multi_player);
            if(!j_settings->map.count("game_width")) j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(default_settings.game_width);
            if(!j_settings->map.count("game_height")) j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(default_settings.game_height);
            if(!j_settings->map.count("game_window_title")) j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>(default_settings.game_window_title);
            if(!j_settings->map.count("compile_command")) j_settings->map["compile_command"] = gc.New<ufo::gc::JsonString>(default_settings.compile_command);
            if(!j_settings->map.count("start_level")) j_settings->map["start_level"] = gc.New<ufo::gc::JsonString>(default_settings.start_level);

             project_settings.v_sync = (bool)j_settings->map["vsync"]->AsFloat();
             project_settings.multi_player = (bool)j_settings->map["multi_player"]->AsFloat();
             project_settings.game_width = (int)j_settings->map["game_width"]->AsFloat();
             project_settings.game_height = (int)j_settings->map["game_height"]->AsFloat();
             project_settings.game_window_title = j_settings->map["game_window_title"]->AsString();
             project_settings.compile_command = j_settings->map["compile_command"]->AsString();
             project_settings.start_level = j_settings->map["start_level"]->AsString();
        }catch(const std::exception& _error){
            Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
        }
        j_settings->Write(opened_directory_path+"/settings.json");
    }
    else{
        auto j_settings = gc.New<ufo::gc::JsonMap>();
        try{

            ProjectSettings default_settings;

            j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(default_settings.v_sync));
            j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(default_settings.game_width));
            j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(default_settings.game_height));
            j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(default_settings.multi_player));
            j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>(default_settings.game_window_title);
            j_settings->map["compile_command"] = gc.New<ufo::gc::JsonString>(default_settings.compile_command);
            j_settings->map["start_level"] = gc.New<ufo::gc::JsonString>(default_settings.start_level);
        }catch(const std::exception& _error){
            Console::PrintLine("[UFO-Engine Studio] Editor: Somehow failed to write property vsync");
        }
        j_settings->Write(opened_directory_path+"/settings.json");
    }

    finished_loading_folder = false;
    refresh_entire_project = true;
}

void Editor::RefreshFolder(){

    opened_directory = FileNode::ParseFolder(opened_directory_path);
    opened_directory->file_name = "";

}

void
Editor::Load(){
    //OpenFolder("/home/uj/Documents/C++/sta_replica");

    /*ufo::gc::JsonMap* json_config = gc.New<ufo::gc::JsonMap>();
    ufo::gc::JsonArray* json_array = gc.New<ufo::gc::JsonArray>();
    for(const std::string& recently_opened_path : recently_opened){
        json_array->array.push_back(gc.New<ufo::gc::JsonString>(recently_opened_path));
    }
    json_config->map.emplace("recently_opened", json_array);
    json_config->Write("../editor_config.json");*/

    if(ufo::FileSystem::FileExists("../editor_config.json")){
        ufo::gc::JsonMap* json_config = ufo::gc::JsonRead(&gc, "../editor_config.json");
        if(json_config->map.count("recently_opened")) for(const auto& recently_opened_path : json_config->map.at("recently_opened")->AsArray()){
            if(ufo::FileSystem::FileExists(recently_opened_path->AsString())) recently_opened.insert(recently_opened_path->AsString());
        }
    }

}

void Editor::ImportHeaderFileToProject(std::string _path){

}

void Editor::OnUpdate(float _delta_time){

    if(!finished_loading_folder){
        ResetUFOEngineStudio();
        finished_loading_folder = true;
    }

    if(!finished_importing_assets){
        for(const std::string& texture : queued_textures){
            engine->asset_manager.OnAddTexture(texture, this);
        }

        for(const std::string& shader_folder_full_path : queued_shaders){

            try{
                const std::string relative_path = ufo::FileSystem::GetRelativePath(shader_folder_full_path,opened_directory_path);

                const std::string vertex_shader_path = shader_folder_full_path+"/vertex.glsl";
                const std::string fragment_shader_path = shader_folder_full_path+"/fragment.glsl";
                const std::string geometry_shader_path = shader_folder_full_path+"/geometry.glsl"; //Unused for now

                bool shader_loaded_successfully = engine->asset_manager.LoadShader(vertex_shader_path.c_str(), fragment_shader_path.c_str(), nullptr, relative_path);

                if(!shader_loaded_successfully){
                    Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Failed to load shader",shader_folder_full_path);
                    continue;
                }

                glm::mat4 projection = glm::ortho(
                    0.0f, static_cast<float>(engine->width),
                    static_cast<float>(engine->height), 0.0f,
                    -1.0f, 0.0f
                );

                engine->asset_manager.GetShader(relative_path).Use();
                engine->asset_manager.GetShader(relative_path).SetInt("image", 0);
                engine->asset_manager.GetShader(relative_path).SetMatrix4("projection", projection);

            } catch (const std::runtime_error& _error){
                Console::PrintLine(__UFO_PRETTY_FUNCTION__, _error.what());
            }
        }

        for(const std::string& tileset : queued_tilesets){
            auto level_editor = dynamic_cast<LevelEditorTab*>(active_tab);
            level_editor->this_level->tileset_manager.AddTileset(tileset, level_editor);
        }

        finished_importing_assets = true;
        queued_shaders.clear();
        queued_textures.clear();
        queued_tilesets.clear();
    }

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
    if(opened_directory != nullptr){
        opened_directory->Update(0, nullptr, "", this);

        opened_directory->AddFileNodesRecursive();
    }
    ImGui::End();

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Crash Test")) throw std::runtime_error("Crash Test");

            if (ImGui::BeginMenu("Open Recent"))
            {
                for(const std::string& recent : recently_opened){
                    if(ImGui::MenuItem(recent.c_str())){
                        if(ufo::FileSystem::FileExists(recent)) OpenFolder(recent);
                    }
                }
                ImGui::EndMenu();
            }


            if(ImGui::MenuItem("Open Folder")){
                SDL_ShowOpenFolderDialog(&UFOEngineStudio::OnOpenFolder, this, engine->window, "/home", false);
            }
            if(opened_directory_path != ""){
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
                        tab->this_level->editor_name = "Root";
                        tabs.push_back(std::move(tab));
                    }
                    if(ImGui::MenuItem("Textfile (.txt)")){
                        tabs.push_back(std::make_unique<TextEditorTab>("","",this));
                    }
                    if(ImGui::MenuItem("C++ class (.cpp)")){

                        std::string template_file_header = ufo::FileSystem::Read("../UFO-Engine/project_templates/my_actor.h");
                        std::string template_file_source = ufo::FileSystem::Read("../UFO-Engine/project_templates/my_actor.cpp");

                        tabs.push_back(std::make_unique<TextEditorTab>("",template_file_header,this));
                        tabs.push_back(std::make_unique<TextEditorTab>("",template_file_source,this));
                    }

                    ImGui::EndMenu();
                }
            }


            ImGui::EndMenu();
        }
        if(opened_directory_path != ""){
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

                if(ImGui::MenuItem("Make release build")){
                    SDL_ShowOpenFolderDialog(&OnSelectDirectoryForDebugBuild, (void*)this, engine->window, "/home", false);
                }

                ImGui::EndMenu();
            }

            if(active_tab){
                active_tab->TabSpecificMainMenuBarItems();
            }
        }

        if(ImGui::BeginMenu("Tools")){
            if(ImGui::MenuItem("Calculator")){
                view_calculator = true;
            }

            ImGui::EndMenu();
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

        if(ImGui::Button("Select Start Level")){
            SDL_ShowOpenFileDialog(&OnSelectStartLevel, this, engine->window, nullptr, 0, opened_directory_path.c_str(), false);
        }
        ImGui::SameLine(); ImGui::Text("%s", project_settings.start_level.c_str());

        ImGui::InputTextMultiline("Compile command:", &project_settings.compile_command);

        if(ImGui::Button("Apply & Save")){
            if(ufo::FileSystem::FileExists(opened_directory_path+"/settings.json")){
                auto j_settings = ufo::gc::JsonRead(&gc, opened_directory_path+"/settings.json");
                try{
                    if(!j_settings->map.count("vsync")) j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(0.0f);
                    if(!j_settings->map.count("multi_player")) j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(0.0f);
                    if(!j_settings->map.count("game_width")) j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(1600.0f);
                    if(!j_settings->map.count("game_height")) j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(900.0f);
                    if(!j_settings->map.count("game_window_title")) j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>("");
                    if(!j_settings->map.count("compile_command")) j_settings->map["compile_command"] = gc.New<ufo::gc::JsonString>("");
                    if(!j_settings->map.count("start_level")) j_settings->map["start_level"] = gc.New<ufo::gc::JsonString>("");

                    j_settings->map["vsync"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.v_sync));
                    j_settings->map["game_width"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_width));
                    j_settings->map["game_height"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.game_height));
                    j_settings->map["multi_player"] = gc.New<ufo::gc::JsonNumber>(int(project_settings.multi_player));
                    j_settings->map["game_window_title"] = gc.New<ufo::gc::JsonString>(project_settings.game_window_title);
                    j_settings->map["compile_command"] = gc.New<ufo::gc::JsonString>(project_settings.compile_command);
                    j_settings->map["start_level"] = gc.New<ufo::gc::JsonString>(project_settings.start_level);
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
                    j_settings->map["compile_command"] = gc.New<ufo::gc::JsonString>(project_settings.compile_command);
                    j_settings->map["start_level"] = gc.New<ufo::gc::JsonString>(project_settings.start_level);
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

    error_dialogue->Update(this);

    //UFOEngineStudio::ImGuiDockSpaceSplit(dock_space_id, viewport->Size, "File Tree", "TabBarWindow", UFOEngineStudio::SplitDirections::HORIZONTAL);

    if(opened_directory_path != "" && refresh_entire_project){
        //Here I'm forcing an update on the new actor queue to make sure there are actors in level are loaded before
        // making potential modifications to them, like adding or removing ufo-variables.
        AddNewActors();

        spawnable_actor_map.clear();
#ifdef __MINGW32__
        Console::PrintLine("__MINGW32__");
        [[maybe_unused]] int execution_fail = std::system(std::string(std::string("cd "++ std::filesystem::current_path().generic_string()+"/UFO-Engine/header_tool && " + std::filesystem::current_path().generic_string() + "/build/python.exe "+header_tool_parser + " ")+std::string("\"")+opened_directory_path+std::string("\"")).c_str());
#else
        [[maybe_unused]] int execution_fail = std::system(std::string(std::string("cd ../UFO-Engine/header_tool && python3 "+header_tool_parser + " ")+std::string("\"")+opened_directory_path+std::string("\"")).c_str());
#endif
        engine->actor_generator->InitialiseActorClassJsons(opened_directory_path);
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

        const std::string build_directory = opened_directory_path + "/build";
        std::thread t(&BuildAndRunProgram, this, build_directory, opened_directory_path);
        t.detach();
        will_compile_game = false;
    }

    gc.Collect();
}

void Editor::PopulateSpawnableActorMapWithBaseObjects(){
    spawnable_actor_map.emplace("ufo::Actor",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Actor>(Vector2f(0.0f, 0.0f));
        }, "ufo::Actor", "ufo::Actor", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::AnimationCluster",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::AnimationCluster>(Vector2f(0.0f, 0.0f));
        }, "ufo::AnimationCluster", "ufo::AnimationCluster", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::CollisionGrid",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::CollisionGrid>(Vector2f(0.0f, 0.0f));
        }, "ufo::CollisionGrid", "ufo::CollisionGrid", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::PlatformerRectangleCollision",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::PlatformerRectangleCollision>(Vector2f(0.0f, 0.0f));
        }, "ufo::PlatformerRectangleCollision", "ufo::PlatformerRectangleCollision", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::TileMap",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::TileMap>(Vector2f(0.0f, 0.0f));
        }, "ufo::TileMap", "ufo::TileMap", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::Level",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Level>();
        }, "ufo::Level", "ufo::Level", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::Widget",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Widget>(Vector2f(0.0f, 0.0f));
        }, "ufo::Widget", "ufo::Widget", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::Text",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Text>(Vector2f(0.0f, 0.0f));
        }, "ufo::Text", "ufo::Text", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::Button",std::move(std::make_unique<AdvancedActorSpawner>(
        [](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Button>(Vector2f(0.0f, 0.0f));
        }, "ufo::Button", "ufo::Button", "UFO-Engine"))
    );

    spawnable_actor_map.emplace(
        "ufo::Sprite",
        std::move(std::make_unique<AdvancedActorSpawner>(
            [](Editor* _editor, AdvancedActorSpawner* _this){
                return std::make_unique<ufo::Sprite>(
                    Vector2f(0.0f, 0.0f),
                    "placeholder_icon",
                    Vector2f(0.0f, 0.0f),
                    Vector2f(32.0f, 32.0f),
                    Vector2f(1.0f, 1.0f),
                    0.0f,
                    0
                );
            },
            "ufo::Sprite",
            "ufo::Sprite", "UFO-Engine"
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
            "ufo::BackgroundSprite", "UFO-Engine"
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
            "ufo::Animation", "UFO-Engine"
        ))
    );

    spawnable_actor_map.emplace("ufo::Camera",
        std::move(std::make_unique<AdvancedActorSpawner>([](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::Camera>(Vector2f(0.0f, 0.0f));
        },
        "ufo::Camera", "ufo::Camera", "UFO-Engine"))
    );

    spawnable_actor_map.emplace("ufo::RectangularArea",
        std::move(std::make_unique<AdvancedActorSpawner>([](Editor* _editor, AdvancedActorSpawner* _this){
            return std::make_unique<ufo::RectangularArea>(Vector2f(0.0f, 0.0f));
        },
        "ufo::RectangularArea", "ufo::RectangularArea", "UFO-Engine"))
    );
}

void Editor::ReloadSpawnableActorMap(){

    const std::string structured_classes_full_path = opened_directory_path+"/structured_classes.json";

    if(!ufo::FileSystem::FileExists(structured_classes_full_path)) return;

    auto exported_actors_json = ufo::gc::JsonRead(&gc, structured_classes_full_path);

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

                            return instance;
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

            if(member->AsArray().size() != 2){
                Console::PrintLine(__UFO_PRETTY_FUNCTION__,
                    "Error, members is incorrectly structured, should have two sections, one for macros and one for the variable type, name and value.");
                continue;
            }

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

                if(macro_name == "ufo_alias"){
                    if(args.size() == 1) alias = args[0]->AsString();
                    else Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error, ufo_alias requires 1 argument, here it takes", args.size());
                }

                if(macro_name == "ufo_int_slider"){

                    if(args.size() == 2){
                        act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyIntSlider>(
                            name,
                            alias,
                            std::stoi(value),
                            std::stoi(args[0]->AsString()),
                            std::stoi(args[1]->AsString())
                        ));
                    }
                    else{
                        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error, ufo_int_slider requires 2 arguments, here it takes", args.size());
                    }
                }

                if(macro_name == "ufo_float_slider"){

                    if(args.size() == 3){
                        act_spawner->custom_properties.push_back(std::make_unique<ufo::EditorPropertyFloatSlider>(
                            name,
                            alias,
                            std::stof(value),
                            std::stof(args[0]->AsString()),
                            std::stof(args[1]->AsString()),
                            std::stof(args[2]->AsString())
                        ));
                    }
                    else{
                        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "Error, ufo_float_slider requires 3 arguments, here it takes", args.size());
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

    for(const auto& [k_class_name,v_spawner] : spawnable_actor_map){

        Console::PrintLine("spawnable_actor_map",k_class_name);

        if(engine->actor_generator->actor_jsons_with_unaltered_default_properties.count(k_class_name)){
            ufo::gc::JsonMap* j_actor = engine->actor_generator->actor_jsons_with_unaltered_default_properties.at(k_class_name);

            if(engine->actor_generator->GetBaseClassOf(k_class_name) != j_actor->AsMap().at("class_name")->AsString()){
                error_dialogue = std::make_unique<ErrorDialogueMismatchedClass>( opened_directory_path+"/"+v_spawner->actor_config_path, j_actor->AsMap().at("class_name")->AsString(), k_class_name, engine->actor_generator->GetBaseClassOf(k_class_name));
            }
        }

        if(!spawnable_actor_map.count(v_spawner->base)){
            error_dialogue = std::make_unique<InheritsFromUnknownClass>(k_class_name, v_spawner->base);
        }
    }

}

void Editor::OnMark() {
    for(const auto& [k,v] : spawnable_actor_map){
        v->Mark();
    }
}

void BuildAndRunProgram(Editor* _editor, const std::string& _build_directory, const std::string& _opened_directory_path){

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

    try{
        if(!ufo::FileSystem::FileExists(_build_directory)) std::filesystem::create_directory(_build_directory.c_str());

    } catch(const std::exception& _error){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, _error.what());
    }

    //Could build with max available CPU here.
#ifdef __MINGW32__
    int success = std::system(std::string("cd "+_build_directory+" && cmd \""+_editor->project_settings.compile_command+" && \"Press any key to continue...\" && read p\"").c_str());
#else
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \""+_editor->project_settings.compile_command+" && echo 'Press any key to continue...' && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Project Process Success?", success);
#endif

}

void DebugGame(const std::string& _build_directory, [[maybe_unused]] const std::string& _opened_directory_path){
    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"gdb OUT && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Game Run Success?", success);
}

void RunGame(const std::string& _build_directory, [[maybe_unused]] const std::string& _opened_directory_path){
    //Could build with max available CPU here.
    int success = std::system(std::string("cd "+_build_directory+" && gnome-terminal -- bash -c \"./OUT && echo \"\"Press any key to continue...\"\" && read p\"").c_str());
    Console::PrintLine("[UFO-Engine Studio] Game Run Success?", success);
}

}
