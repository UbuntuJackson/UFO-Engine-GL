#include <algorithm>
#include "level_editor_tab.h"
#include "../ufo_engine_studio/tab.h"
#include "../ufo_engine_studio/editor.h"
#include <engine.h>
#include <memory>
#include <openglv4_5_graphics.h>
#include "../src/level.h"
#include "../src/actor.h"
#include <SDL3/SDL.h>
#include "../ufo_engine_studio/utility_objects/controllable_camera.h"
#include "../imgui/imgui.h"
#include <gc_json.h>
#include <vector>
#include "actor_undo_and_redo.h"
#include "console.h"
#include "dock_utils.h"
#include "file_dialogue.h"
#include "editor.h"
#include "imgui_utils.h"
#include "../ufo_maths/ufo_maths.h"
#include "utility_objects/spawn_cursor.h"
#include "im_vec.h"

namespace UFOEngineStudio{

LevelEditorTab::LevelEditorTab(ufo::Engine* _engine, Editor* _editor) : Tab(_editor), engine{_engine}{
    this_level = _editor->AddActor<ufo::Level>();
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
    Vector2f position = this_level->active_camera_handles.back()->Transform(_position);
    return level_viewport_position + position;
}

void  LevelEditorTab::Initialise(){
    this_level->AddActor<ControllableCamera>(Vector2f(0.0f, 0.0f));
    this_level->is_top_actor_in_editor = true;
    this_level->unremovable = true;

    spawn_cursor = this_level->AddActor<SpawnCursor>(Vector2f(0.0f, 0.0f));
    spawn_cursor->is_savable = false;
    spawn_cursor->editor_name = "SpawnCursor (Editor Tool)";
    spawn_cursor->unremovable = true;

    currently_edited_actor_in_viewport = this_level;

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

    if(ImGui::BeginTabBar("MyResourcesTabBar")){

        if(ImGui::BeginTabItem("Actors")){

            std::map<std::string, std::vector<UFOEngineStudio::AdvancedActorSpawner*>> categories;

            for(const auto& [k,v] : _editor->spawnable_actor_map){
                if(!categories.count(v->category)) categories.emplace(v->category, std::vector<UFOEngineStudio::AdvancedActorSpawner*>{});
                categories.at(v->category).push_back(v.get());
            }

            if(ImGui::Button("Select")){
                current_tool = Tools::SELECT;
                spawn_cursor->actors.clear();
            }

            ImGui::SameLine();

            if(ImGui::Button("Erase")){
                current_tool = Tools::ERASE;
            }

            ImGui::Separator();

            if(ImGui::BeginChild("MyAssetsChildWindow")){

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

            }

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        ufo::Actor* inspected_actor = this_level->GetInspectedActor();

        if(inspected_actor){
            inspected_actor->OnUtiliseAssetManager(this);
        }

        ImGui::EndTabBar();

    }

    ImGui::End();

    ImGui::Begin(std::string("ActorTree###ActorTree"+std::to_string(id)).c_str());

    //I don't want a class to necessarily require a .ason, so for how you don't specity the header file in the .ason, but the .ason in the header file
    //ImGui::Button("HeaderFile"); ImGui::SameLine(); ImGui::Text("%s", header_file.c_str());

    this_level->UpdateEditorTree(_editor, this, 0);

    ImGui::End();

    ImGui::Begin(std::string(currently_viewed_properties_actor_name+"###Properties"+std::to_string(id)).c_str());
    this_level->ViewProperties(this, 0);
    ImGui::End();

    ImGui::Begin(std::string("LevelViewport###LevelViewport"+std::to_string(id)).c_str(), nullptr);

    this_level->UpdatePhase(_delta_time);
    LevelDrawPhase(engine->graphics.get());

    float w_h_ratio = (float)ImGui::GetWindowSize().x/(float)ImGui::GetWindowSize().y;

    ImGui::Image(
        (void*)(intptr_t)(dynamic_cast<ufo::OpenGLv4_5_Graphics*>(engine->graphics.get())->texture_id),
        ImVec2(ImGui::GetWindowSize().x,ImGui::GetWindowSize().y),
        ImVec2(0,0),
        ImVec2(1,-1)
    );

    ImVec2 im_level_viewport_pos = ImGui::GetItemRectMin();

    level_viewport_position = Vector2f(im_level_viewport_pos.x, im_level_viewport_pos.y);

    this_level->UpdateEditorViewport(editor, this);

    bool is_viewport_hovered = ImGui::IsItemHovered();

    {
        //Relative to computer screen
        ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

        //Relative to computer screen
        ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

        //Relative to SDL window origo
        Vector2f editor_viewport_pos = Vector2f(im_viewport_pos.x-window_pos.x,im_viewport_pos.y-window_pos.y);

        //Name suggestion: mouse_position_over_level_viewport_screenspace
        mouse_position_over_screenspace = ((engine->mouse.position)-editor_viewport_pos)*window_to_engine_ratio;
        former_mouse_position_over_screenspace = ((engine->mouse.former_position)-editor_viewport_pos)*window_to_engine_ratio;
    }

    //This needs to run regardless of whether is_viewport_hovered is true or not
    if(show_multi_selection_right_click_pop_up_menu){

        if(ImGui::BeginPopupContextItem("show_multi_selection_right_click_pop_up_menu")){

            if(ImGui::MenuItem("Delete")){

                this_level->RemoveFutureChanges();

                auto remove_actor_change = std::make_unique<ufo::ActorChange_RemoveMultipleActors>();

                for(const auto& actor : selected_actors){

                    remove_actor_change->changes.push_back(ufo::ActorChange_RemoveActor(actor));

                    actor->stash = true;

                }

                this_level->level_changes.push_back(std::move(remove_actor_change));

                show_multi_selection_right_click_pop_up_menu = false;

                selected_actors.clear();
            }

            ImGui::EndPopup();
        }

    }

    if(is_viewport_hovered) SelectionUpdate();

    ImGui::End();
}

void LevelEditorTab::SelectionUpdate(){

    if(engine->mouse.is_right_button_pressed){
        for(const auto& actor : selected_actors){

            ufo::Actor* act = actor->OnGetFocusedActor(mouse_position_over_screenspace);

            if(act){
                show_multi_selection_right_click_pop_up_menu = true;
            }
        }
    }

    if(engine->mouse.is_left_button_pressed){
        for(const auto& actor : selected_actors){
            ufo::Actor* act = actor->OnGetFocusedActor(mouse_position_over_screenspace);

            if(act){
                current_tool = Tools::MOVE_ACTOR_CLUSTER;

                this_level->RemoveFutureChanges();

                std::unique_ptr<ufo::ActorChange_MultipleActorChange> multiple_actor_change = std::make_unique<ufo::ActorChange_MultipleActorChange>();

                for(const auto& actor : selected_actors){

                    multiple_actor_change->changes.push_back(
                        std::make_unique<ufo::ActorChange_CustomVariableVector2fHandle>(&actor->local_position,actor->local_position,Vector2f(0.0f, 0.0f))
                    );
                }

                this_level->level_changes.push_back(std::move(multiple_actor_change));

                break;
            }
        }
    }

    if(current_tool == Tools::MOVE_ACTOR_CLUSTER){
        Vector2f dp = engine->mouse.position - engine->mouse.former_position;

        for(const auto& actor : selected_actors){

            actor->local_position += dp;

        }
    }

    if(current_tool == Tools::MOVE_ACTOR_CLUSTER && engine->mouse.is_left_button_released){
        ufo::ActorChange_MultipleActorChange* movement_change = dynamic_cast<ufo::ActorChange_MultipleActorChange*>(this_level->level_changes.back().get());
        if(movement_change){
            for(size_t a = 0; a < movement_change->changes.size(); a++){
                ufo::ActorChange_CustomVariableVector2fHandle* local_position_change = dynamic_cast<ufo::ActorChange_CustomVariableVector2fHandle*>(movement_change->changes[a].get());

                if(!local_position_change){
                    Console::PrintLine("[UFO-Engine Studio] LevelEditorTab::OnActive: Error converting from ufo::ActorChange to ActorChange_CustomVariableVector2fHandle");
                    throw;
                }

                local_position_change->current_value = selected_actors[a]->local_position;
            }
        }else{
            Console::PrintLine("[UFO-Engine Studio] LevelEditorTab::OnActive: Error converting from ufo::ActorChange to ActorChange_MultipleActorChange");
            throw;
        }

        for(const auto& actor : selected_actors){
            auto local_tile_map = actor->IsInTileMap();

            if(local_tile_map){

                actor->local_position = Vector2f(
                    std::floor(actor->local_position.x/local_tile_map->tile_width)*local_tile_map->tile_width,
                    std::floor(actor->local_position.y/local_tile_map->tile_height)*local_tile_map->tile_height);
            }
        }

        Console::PrintLine("Set tool Tools::MULTI_SELECT");
        current_tool = Tools::SELECT;

    }

    if(current_tool == Tools::MOVE_ACTOR_CLUSTER) return;

    if(engine->mouse.is_left_button_pressed){
        rectangle_selection_tool_start_position = engine->mouse.position;
    }

    ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

    Vector2f editor_viewport_pos = Vector2f(level_viewport_position.x-window_pos.x,level_viewport_position.y-window_pos.y);

    //Resize selection rectangle if mouse is moving
    if(engine->mouse.is_left_button_held && engine->mouse.delta_position != Vector2f(0.0f, 0.0f)){
        ufo::Rectangle selected_rectangle = GetSelectionRectangle();

        Vector2f world_selection_rectangle_start_editor_viewport = ((selected_rectangle.position)                        -editor_viewport_pos)*window_to_engine_ratio;
        Vector2f world_selection_rectangle_end_editor_viewport   = ((selected_rectangle.position+selected_rectangle.size)-editor_viewport_pos)*window_to_engine_ratio;

        Vector2f world_selection_rectangle_start = this_level->active_camera_handles.back()->TransformScreenToWorld(world_selection_rectangle_start_editor_viewport);
        Vector2f world_selection_rectangle_end   = this_level->active_camera_handles.back()->TransformScreenToWorld(world_selection_rectangle_end_editor_viewport);

        selection_rectangle_world_space = ufo::Rectangle(world_selection_rectangle_start, world_selection_rectangle_end - world_selection_rectangle_start);

        Console::PrintLine("clearing");
        selected_actors.clear();
        this_level->GetSelectedActors(selected_actors, selection_rectangle_world_space);

        ImGui::GetWindowDrawList()->AddRectFilled(
            FromVector2fToImVec2((selected_rectangle.position+FromImVec2ToVector2f(ImGui::GetMainViewport()->Pos))),
            FromVector2fToImVec2((selected_rectangle.position+selected_rectangle.size+FromImVec2ToVector2f(ImGui::GetMainViewport()->Pos))), 0x55555555);
    }

    /*if(current_tool == Tools::SELECT){
        ufo::Actor* focused_actor = this_level->GetFocusedActor(mouse_position_over_screenspace);

        if(focused_actor && engine->mouse.is_left_button_pressed){
            selected_actors.clear();
            selected_actors.push_back(focused_actor);
        }
    }*/

    for(ufo::Actor* _actor : selected_actors){
        const Vector2f pos_min = TranslateToEditorScreenSpace(_actor->GetGlobalPosition())+_actor->editor_hitbox.position;
        const Vector2f pos_max = TranslateToEditorScreenSpace(_actor->GetGlobalPosition())+_actor->editor_hitbox.position+_actor->editor_hitbox.size;

        ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min), UFOEngineStudio::FromVector2fToImVec2(pos_max), 0xFFFFFFFF);
    }

    if(selected_actors.size() > 0){
        //-1 is just a temporary index here to test things out, I don't think that value is actually used to anything.
        selected_actors[0]->ViewProperties(this,-1);
    }
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
    //Binding custom buffer to draw stuff in editor level viewport
    _graphics->BindFrameBuffer();

    //I think this part is leaking memory somehow
    _graphics->RescaleFrameBuffer(ImGui::GetWindowSize().x,ImGui::GetWindowSize().y);

    glClearColor(0.0125f, 0.025f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this_level->DrawPhase(_graphics,ImGui::GetWindowSize().x,ImGui::GetWindowSize().y);
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
