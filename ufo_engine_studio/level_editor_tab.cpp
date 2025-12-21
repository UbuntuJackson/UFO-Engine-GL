#include "level_editor_tab.h"
#include "../ufo_engine_studio/tab.h"
#include "../ufo_engine_studio/editor.h"
#include <engine.h>
#include <sprite_renderer.h>
#include <level.h>
#include <SDL3/SDL.h>
#include "../ufo_engine_studio/utility_objects/controllable_camera.h"
#include "../imgui/imgui.h"
#include <gc_json.h>
#include "dock_utils.h"
#include "file_utilities.h"
#include "editor.h"

namespace UFOEngineStudio{

LevelEditorTab::LevelEditorTab(ufo::Engine* _engine, Editor* _editor) : Tab(_editor), engine{_engine}{
    this_level = _editor->AddActor<Level>();
    this_level->AddActor<ControllableCamera>(Vector2f(0.0f, 0.0f));
    this_level->Load();
}

void LevelEditorTab::Refresh(){
    Tab::Refresh();
    this_level->RemoveAndAddEditorPropertiesDuringRuntime(editor);
}

void LevelEditorTab::OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time){

    if(_editor->set_all_actors_properties_open_to_false){
        this_level->OpenProperties();
    }
    _editor->set_all_actors_properties_open_to_false = false;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_AlwaysVerticalScrollbar;

    ImGui::Begin(std::string("ContentBrowser###ContentBrowser"+std::to_string(id)).c_str());

    ImGuiID content_browser_dockspace_id = ImGui::GetID(std::string("ContentBrowser###ContentBrowser"+std::to_string(id)).c_str());
    //ImGuiDockSpaceFill(content_browser_dockspace_id, ImGui::GetWindowSize(), std::string("ActorTree###ActorTree"+std::to_string(id)).c_str());

    ImGuiDockSpaceSplit(
        content_browser_dockspace_id,
        ImGui::GetWindowSize(),
        std::string("ActorTree###ActorTree"+std::to_string(id)).c_str(),
        std::string(currently_viewed_properties_actor_name+"###Properties"+std::to_string(id)).c_str(),
        SplitDirections::VERTICAL);

    ImGui::DockSpace(content_browser_dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    ImGui::Begin(std::string("ActorTree###ActorTree"+std::to_string(id)).c_str());

    //I don't want a class to necessarily require a .ason, so for how you don't specity the header file in the .ason, but the .ason in the header file
    //ImGui::Button("HeaderFile"); ImGui::SameLine(); ImGui::Text("%s", header_file.c_str());

    this_level->UpdateEditorTree(_editor, 0);

    ImGui::End();

    ImGui::Begin(std::string(currently_viewed_properties_actor_name+"###Properties"+std::to_string(id)).c_str());
    this_level->ViewProperties(this, 0);
    ImGui::End();

    ImGui::Begin(std::string(name_and_imgui_id.c_str()+std::to_string(id)).c_str(), nullptr, window_flags);

    LevelUpdatePhase(_delta_time);
    LevelDrawPhase(engine->graphics.get());

    ImGui::Image(
        (void*)(intptr_t)(dynamic_cast<ufo::OpenGLv4_5_Graphics*>(engine->graphics.get())->texture_id),
        ImVec2(engine->width, engine->height),
        ImVec2(0,0),
        ImVec2(1,-1)
    );

    ImGui::End();
}

void LevelEditorTab::OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state){
    ImGuiDockSpaceSplit(
        _local_dockspace_id,
        ImGui::GetWindowSize(),
        std::string(name_and_imgui_id.c_str()+std::to_string(id)).c_str(),
        std::string("ContentBrowser###ContentBrowser"+std::to_string(id)).c_str(),
        SplitDirections::HORIZONTAL);
}

void LevelEditorTab::LevelUpdatePhase(float _delta_time){
    this_level->UpdatePhrase(_delta_time);
}

void LevelEditorTab::LevelDrawPhase(ufo::Graphics* _graphics){
    _graphics->BindFrameBuffer();

    glViewport(0, 0, this_level->size.x, this_level->size.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this_level->DrawPhase(_graphics);

    _graphics->UnbindFrameBuffer();
}

void LevelEditorTab::OnSave(Editor* _editor){

    if(path != ""){
        auto level_json = this_level->GetAsJson(&gc);
        level_json->Write(path);
    }
    else{
        const char* global_file_location = _editor->opened_directory_path.c_str();

        SDL_ShowSaveFileDialog(&OnNewActorFile , this, _editor->engine->window, nullptr, 0, global_file_location);
    }

    _editor->should_refresh_working_directory = true;
    Refresh();
}

}
