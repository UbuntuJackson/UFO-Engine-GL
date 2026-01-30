#include <algorithm>
#include "level_editor_tab.h"
#include "../ufo_engine_studio/tab.h"
#include "../ufo_engine_studio/editor.h"
#include <engine.h>
#include <openglv4_5_graphics.h>
#include <level.h>
#include <SDL3/SDL.h>
#include "../ufo_engine_studio/utility_objects/controllable_camera.h"
#include "../imgui/imgui.h"
#include <gc_json.h>
#include "dock_utils.h"
#include "file_dialogue.h"
#include "editor.h"
#include "imgui_utils.h"
#include "../ufo_maths/ufo_maths.h"
#include "utility_objects/spawn_cursor.h"
#include "im_vec.h"

namespace UFOEngineStudio{

LevelEditorTab::LevelEditorTab(ufo::Engine* _engine, Editor* _editor) : Tab(_editor), engine{_engine}{
    this_level = _editor->AddActor<Level>();
    this_level->Load();
}

ufo::Rectangle LevelEditorTab::GetSelectionRectangle(){
    float start_x = rectangle_selection_tool_start_position.x;
    float start_y = rectangle_selection_tool_start_position.y;

    float end_x = engine->mouse.position.x;
    float end_y = engine->mouse.position.y;

    if(start_y > end_y) std::swap(start_y, end_y);
    if(start_x > end_x) std::swap(start_x, end_x);

    return ufo::Rectangle(Vector2f(start_x,start_y), Vector2f(end_x-start_x, end_y-start_y));

}

Vector2f LevelEditorTab::TranslateToEditorScreenSpace(Vector2f _position){
    Vector2f position = this_level->active_camera_handles.back()->Transform(_position)/window_to_engine_ratio;
    return level_viewport_position + position;
}

void  LevelEditorTab::Initialise(){
    this_level->AddActor<ControllableCamera>(Vector2f(0.0f, 0.0f));
    this_level->is_top_actor_in_editor = true;
    this_level->UpdateActorStructureFirstFrame(editor, false);
    this_level->unremovable = true;

    spawn_cursor = this_level->AddActor<SpawnCursor>(Vector2f(0.0f, 0.0f));
    spawn_cursor->is_savable = false;
    spawn_cursor->editor_name = "SpawnCursor (Editor Tool)";
    spawn_cursor->unremovable = true;

    //this_level->UpdatePhase(0.016f);
}

void LevelEditorTab::Refresh(){
    Tab::Refresh();
    Console::PrintLine("[UFO-Engine Studio] LevelEditorTab::Refresh: Updating actor structure");
    this_level->UpdateActorStructure(editor, false);
    this_level->RemoveAndAddEditorPropertiesDuringRuntime(editor);
}

void LevelEditorTab::OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time){

    if(_editor->set_all_actors_properties_open_to_false){
        this_level->OpenProperties();
    }
    _editor->set_all_actors_properties_open_to_false = false;

    if(reset_selection_status){
        this_level->ResetSelectionStatus();
    }
    reset_selection_status = false;

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

    ImGui::DockSpace(content_browser_dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoTabBar);

    ImGui::End();

    ImGui::Begin(std::string(name_and_imgui_id.c_str()+std::to_string(id)).c_str());

    ImGuiID level_viewport_dockspace_id = ImGui::GetID(std::string("LevelViewport###LevelViewport"+std::to_string(id)).c_str());
    //ImGuiDockSpaceFill(content_browser_dockspace_id, ImGui::GetWindowSize(), std::string("ActorTree###ActorTree"+std::to_string(id)).c_str());

    ImGuiDockSpaceSplit(
        level_viewport_dockspace_id,
        ImGui::GetWindowSize(),
        std::string("LevelViewport###LevelViewport"+std::to_string(id)).c_str(),
        std::string("ActorPicker###ActorPicker"+std::to_string(id)),
        SplitDirections::VERTICAL);

    ImGui::DockSpace(level_viewport_dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_NoTabBar);

    ImGui::End();

    ImGui::Begin(std::string("ActorPicker###ActorPicker"+std::to_string(id)).c_str());

    std::map<std::string, std::vector<UFOEngineStudio::Editor::AdvancedActorSpawner*>> categories;

    for(const auto& [k,v] : _editor->spawnable_actor_map){
        if(!categories.count(v->category)) categories.emplace(v->category, std::vector<UFOEngineStudio::Editor::AdvancedActorSpawner*>{});
        categories.at(v->category).push_back(v.get());
    }

    ImGui::SameLine();

    if(ImGui::Button("Select")){
        current_tool = Tools::SELECT;
        spawn_cursor->actors.clear();
    }

    ImGui::SameLine();

    if(ImGui::Button("Erase")){
        current_tool = Tools::ERASE;
    }

    ImGui::Separator();

    if (ImGui::BeginTable("table_columns_flags_checkboxes", categories.size(), ImGuiTableFlags_None))
    {
        UFOEngineStudio::PushStyleCompact();
        for(const auto& [k,v] : categories){
            ImGui::TableNextColumn();
            ImGui::Text("%s",k.c_str());

            for(const auto& s : v){
                int w = engine->asset_manager.textures.at("actor_icon").width;
                int h = engine->asset_manager.textures.at("actor_icon").height;

                bool pressed = ImGui::ImageButton(

                    std::string("Add "+s->class_name+"###Add"+k+s->class_name).c_str(),
                    (ImTextureID)(intptr_t)engine->asset_manager.textures.at("actor_icon").id,
                    ImVec2(w, h));

                if(editor->currently_selected_actor_type == s->class_name){
                    ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), 0x55FFFFFF);
                }

                if(pressed){
                    editor->currently_selected_actor_type = s->class_name;
                    current_tool = Tools::PLACE;

                    spawn_cursor->actors.clear();
                    spawn_cursor->AddActorUniquePtr(editor->spawnable_actor_map.at(s->class_name)->Spawn(editor));
                }

                ImGui::SameLine();

                ImGui::TextWrapped("%s", (s->comment == "" ? s->class_name.c_str() : std::string(s->class_name+"\n-- Description --\n"+s->comment)).c_str());
            }
        }
        UFOEngineStudio::PopStyleCompact();

        ImGui::EndTable();
    }

    ImGui::Separator();

    for(const auto& [k,v] : _editor->spawnable_actor_map){
        if(ImGui::Button(std::string("Add "+k).c_str())){
            /*auto inst = v->Spawn(_editor);
            inst->class_name = k;
            AddActorUniquePtr(std::move(inst));*/
        }
    }

    ImGui::End();

    ImGui::Begin(std::string("ActorTree###ActorTree"+std::to_string(id)).c_str());

    //I don't want a class to necessarily require a .ason, so for how you don't specity the header file in the .ason, but the .ason in the header file
    //ImGui::Button("HeaderFile"); ImGui::SameLine(); ImGui::Text("%s", header_file.c_str());

    this_level->UpdateEditorTree(_editor, 0);

    ImGui::End();

    ImGui::Begin(std::string(currently_viewed_properties_actor_name+"###Properties"+std::to_string(id)).c_str());
    this_level->ViewProperties(this, 0);
    ImGui::End();

    ImGui::Begin(std::string("LevelViewport###LevelViewport"+std::to_string(id)).c_str(), nullptr, ImGuiWindowFlags_AlwaysHorizontalScrollbar);

    this_level->UpdatePhase(_delta_time);
    LevelDrawPhase(engine->graphics.get());

    float w_h_ratio = (float)engine->width/(float)engine->height;



    ImGui::Image(
        (void*)(intptr_t)(dynamic_cast<ufo::OpenGLv4_5_Graphics*>(engine->graphics.get())->texture_id),
        ImVec2(ImGui::GetWindowSize().y*w_h_ratio, ImGui::GetWindowSize().y),
        ImVec2(0,0),
        ImVec2(1,-1)
    );

    //Establish some utility variables for managing positions used for translating, drawing and detecting

    //The level viewport is slightly smaller than the real thing, so this ratio variable is pretty important.
    window_to_engine_ratio = engine->height / ImGui::GetWindowSize().y;

    {
        //Relative to computer screen
        ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

        //Relative to computer screen
        ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

        //Relative to SDL window origo
        Vector2f editor_viewport_pos = Vector2f(im_viewport_pos.x-window_pos.x,im_viewport_pos.y-window_pos.y);

        //This is just im_viewport_pos as a Vector2f, this naming is urgently awful
        level_viewport_position = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

        //Name suggestion: mouse_position_over_level_viewport_screenspace
        mouse_position_over_screenspace = ((engine->mouse.position)-editor_viewport_pos)*window_to_engine_ratio;
        former_mouse_position_over_screenspace = ((engine->mouse.former_position)-editor_viewport_pos)*window_to_engine_ratio;

        spawn_cursor->local_position = this_level->active_camera_handles.back()->TransformScreenToWorld(mouse_position_over_screenspace);
        if(spawn_cursor->actors.size() > 0){
            spawn_cursor->actors[0]->local_position = {0.0f, 0.0f};
        }
    }

    this_level->UpdateEditorViewport(editor, this);
    this_level->UpdateEditorViewportFocus(editor, this);

    //Set initial start position for rectangle selection tool
    if(engine->mouse.is_left_button_pressed){
        rectangle_selection_tool_start_position = engine->mouse.position;
    }

    //Resize selection rectangle if mouse is moving
    if(engine->mouse.delta_position != Vector2f(0.0f, 0.0f) && engine->mouse.is_left_button_held){

        ufo::Rectangle selected_rectangle = GetSelectionRectangle();

        ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

        ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

        Vector2f editor_viewport_pos = Vector2f(im_viewport_pos.x-window_pos.x,im_viewport_pos.y-window_pos.y);

        Vector2f world_selection_rectangle_start_editor_viewport = ((selected_rectangle.position)-editor_viewport_pos)*window_to_engine_ratio;
        Vector2f world_selection_rectangle_end_editor_viewport = ((selected_rectangle.position+selected_rectangle.size)-editor_viewport_pos)*window_to_engine_ratio;

        Vector2f world_selection_rectangle_start = this_level->active_camera_handles.back()->TransformScreenToWorld(world_selection_rectangle_start_editor_viewport);
        Vector2f world_selection_rectangle_end = this_level->active_camera_handles.back()->TransformScreenToWorld((world_selection_rectangle_end_editor_viewport));

        selection_rectangle_world_space = ufo::Rectangle(world_selection_rectangle_start, world_selection_rectangle_end-world_selection_rectangle_start);
        Console::PrintLine(selection_rectangle_world_space.position, selection_rectangle_world_space.size);
    }
    //Set selection tool to inactive if user clicks but does not move the mouse
    if(engine->mouse.delta_position == Vector2f(0.0f, 0.0f) && engine->mouse.is_left_button_pressed){
        selection_rectangle_world_space = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f));
        Console::PrintLine("Deselected everything");
    }

    //If selection tool active
    if(selection_rectangle_world_space.size != Vector2f(0.0f, 0.0f)){

        ufo::Rectangle selected_rectangle = GetSelectionRectangle();
        ImGui::GetWindowDrawList()->AddRectFilled(
            FromVector2fToImVec2((selected_rectangle.position+FromImVec2ToVector2f(ImGui::GetMainViewport()->Pos))),
            FromVector2fToImVec2((selected_rectangle.position+selected_rectangle.size+FromImVec2ToVector2f(ImGui::GetMainViewport()->Pos))), 0x55555555);
        std::vector<Actor*> actors_selected_this_frame;
        this_level->GetSelectedActors(actors_selected_this_frame,selection_rectangle_world_space);
    }

    ImGui::End();
}

void LevelEditorTab::OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state){
    ImGuiDockSpaceSplit(
        _local_dockspace_id,
        ImGui::GetWindowSize(),
        std::string(name_and_imgui_id.c_str()+std::to_string(id)),
        std::string("ContentBrowser###ContentBrowser"+std::to_string(id)),
        SplitDirections::HORIZONTAL);
}

void LevelEditorTab::LevelDrawPhase(ufo::Graphics* _graphics){
    _graphics->BindFrameBuffer();

    glViewport(0, 0, this_level->size.x, this_level->size.y);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this_level->DrawPhase(_graphics);
    this_level->DrawGizmosPhase(editor->engine->graphics.get(), this);

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

    Refresh();
}

}
