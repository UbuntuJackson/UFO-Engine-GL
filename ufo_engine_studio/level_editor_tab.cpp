#include <algorithm>
#include "level_editor_tab.h"
#include "../ufo_engine_studio/tab.h"
#include "../ufo_engine_studio/editor.h"
#include <cstdio>
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
#include "conversion.h"
#include "dock_utils.h"
#include "file_dialogue.h"
#include "editor.h"
#include "file_utils.h"
#include "imgui_utils.h"
#include "../ufo_maths/ufo_maths.h"
#include "../tilemap/tile_map.h"
#include "../src/ufo_macros.h"
#include "utility_objects/spawn_cursor.h"
#include "utility_objects/new_actor_placeholder.h"
#include "im_vec.h"
#include "animation_cluster.h"

#include <unistd.h>

namespace UFOEngineStudio{

void LevelEditorTab::SubmitUndoRedoAction(){

    if(current_undo_redo_action.actor_id != ufo::Maths::NULL_ID){

        bool did_action = this_level->actors_with_stable_id.at(current_undo_redo_action.actor_id)->OnEndUndoRedoAction(this);
        if(did_action){
            this_level->RemoveFutureChanges();
            this_level->level_changes.push_back(std::move(current_undo_redo_action.actor_change));
            this_level->level_changes.back()->Do();
            current_undo_redo_action = UndoRedoAction{ufo::Maths::NULL_ID, Tools::NONE, nullptr};
        }
    }

}

LevelEditorTab::LevelEditorTab(ufo::Engine* _engine, Editor* _editor, bool _is_new_file) : Tab(_editor,_is_new_file), engine{_engine}{

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

void LevelEditorTab::Initialise(ufo::Level* _level, const std::string _local_path){
    this_level = _level;
    this_level->engine = engine;
    path = _local_path;

    this_level->AddActor<ControllableCamera>(Vector2f(0.0f, 0.0f));
    this_level->is_top_actor_in_editor = true;
    this_level->unremovable = true;

    spawn_cursor = this_level->AddActor<SpawnCursor>(Vector2f(0.0f, 0.0f));
    spawn_cursor->is_savable = false;
    spawn_cursor->editor_name = "SpawnCursor (Editor Tool)";
    spawn_cursor->unremovable = true;

    edited_actor_id = this_level->editor_id;

    this_level->tileset_manager.engine = engine;
    this_level->tileset_manager.InitialiseTexturesEditor(editor);

    this_level->actors_with_stable_id.emplace(
        this_level->editor_id, this_level);

}

void LevelEditorTab::Refresh(){
    Tab::Refresh();

    if(ufo::FileSystem::FileExists("header_tool_log_file.txt")){
        Console::PrintLine(ufo::FileSystem::Read("header_tool_log_file.txt"));
    }

    this_level->ResourcesEdited();
    this_level->UpdateActorStructure(editor);
    this_level->RemoveAndAddEditorPropertiesDuringRuntime(editor);
}

void LevelEditorTab::ResourcesEdited(){
    this_level->ResourcesEdited();
}

void LevelEditorTab::TabSpecificMainMenuBarItems(){
    if(ImGui::MenuItem("(<) Undo")){

        this_level->Undo();

    }
    if(ImGui::MenuItem("(>) Redo")){

        this_level->Redo();

    }
    if(ImGui::MenuItem("I> Run Current Level")){
        std::system(("cd "+editor->opened_directory_path+"/build"+" && gnome-terminal -- bash -c './OUT "+editor->opened_directory_path+path+"'").c_str());
    }
}

void LevelEditorTab::OnActive([[maybe_unused]] ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time){

    //Shortcuts for testing purposes
    if(ImGui::IsKeyPressed(ImGuiKey_F5)){
        _editor->refresh_entire_project = true;
    }


    SubmitUndoRedoAction();


    if(reset_selection_status){
        this_level->ResetSelectionStatus();
    }
    reset_selection_status = false;

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

            if(ImGui::Button("Resize")){
                current_tool = Tools::RESIZE;
                spawn_cursor->actors.clear();
            }

            ImGui::SameLine();

            if(ImGui::Button("Erase")){
                current_tool = Tools::ERASE;
            }

            ImGui::Separator();

            if(ImGui::BeginChild("MyAssetsChildWindow")){
                if(categories.size() > 0){
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
                }
            }

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if(inspected_actor_id != ufo::Maths::NULL_ID){
            if(this_level->actors_with_stable_id.at(inspected_actor_id)->import_mode == ufo::Actor::ImportModes::BUILT_IN_CLASS){
                this_level->actors_with_stable_id.at(inspected_actor_id)->OnUtiliseAssetManager(this);
            }
        }

        if(ImGui::BeginTabItem("Output###LevelEditorTabOutput")){
            ImGui::BeginChild("Output###LevelEditorTabOutputChildWindow");

            ImGui::TextWrapped("%s", Console::string_stream.str().c_str());

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Game Output###LevelEditorTabGame Output")){
            if(editor->handle_to_cout_file_descriptor != -1) ImGui::Text("%s","Process ongoing...");
            else ImGui::Text("%s","No ongoing process.");

            #ifdef WIN32
            ImGui::Text("Windows version of output view not implemented.");
            #endif

            ImGui::BeginChild("Game Output###LevelEditorTabGame OutputChildWindow");

            bool added_to_log = false;

            float former_scroll = ImGui::GetScrollY();

            if(editor->handle_to_cout_file_descriptor != -1){

                //fgets_unlocked(buffer, sizeof(buffer), editor->f);

                ssize_t number = read(editor->handle_to_cout_file_descriptor, _editor->game_log_buffer+_editor->game_log_buffer_size, GAME_LOG_BUFFER_SIZE-_editor->game_log_buffer_size);

                if(number > 0) added_to_log = true;

                if(number < 0){
                    Console::PrintLine(__UFO_PRETTY_FUNCTION__);
                    perror("Error");
                }
                else _editor->game_log_buffer_size+=number;

                if(_editor->game_log_buffer_size >= GAME_LOG_BUFFER_SIZE){


                    ssize_t amount_to_keep = _editor->game_log_buffer_size/2;

                    memmove(_editor->game_log_buffer,_editor->game_log_buffer+amount_to_keep, _editor->game_log_buffer_size-amount_to_keep);
                    _editor->game_log_buffer_size = amount_to_keep;
                }

            }

            ImGui::TextUnformatted(_editor->game_log_buffer, _editor->game_log_buffer+_editor->game_log_buffer_size);

            if(added_to_log) ImGui::SetScrollHereY(1.0f);

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("Change log###LevelEditorTabUndoAndRedo")){
            ImGui::BeginChild("Change log###LevelEditorTabLevelEditorTabUndoAndRedoChildWindow");

            ImGui::TextWrapped("Current level change: %s", std::to_string(this_level->current_level_change).c_str());

            for(const auto& change : this_level->level_changes){
                ImGui::TextWrapped("%s", change->GetInfo().c_str());
            }

            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        if(ImGui::BeginTabItem("DragDroppedActors###LevelEditorTabDragDroppedActors")){
            ImGui::BeginChild("DragDroppedActors###LevelEditorTabDragDragDroppedActorsChildWindow");

            for(const auto& actor : drag_dropped_actors) ImGui::TextWrapped("%s", std::string(
                ufo::MemoryAddressToString(actor.parent)+" "+std::to_string(actor.index)
            ).c_str());


            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        /*if(ImGui::BeginTabItem("UFO Visual###UFOVisual")){
            ImGui::BeginChild("Change log###LevelEditorTabLevelEditorTabUndoAndRedoChildWindow");

            vm.main_func.Draw();

            if(ImGui::Button("Run")){
                std::map<std::string, std::unique_ptr<ufo::visual::Block>> no_args;

                vm.main_func.Operation(&vm,no_args);
            }

            ImGui::EndChild();

            ImGui::EndTabItem();
        }*/

        ImGui::EndTabBar();

    }

    ImGui::End();

    ImGui::Begin(std::string("ActorTree###ActorTree"+std::to_string(id)).c_str());

    this_level->UpdateEditorTree(_editor, this, 0);

    /*if(this_level->inserted_actors.size() > 0){
        if(!this_level->moving_actor_with_undo_and_redo) this_level->RemoveFutureChanges();

        std::unique_ptr multiple_actor_change = std::make_unique<ufo::ActorChange_MultipleActorChange>(false);

        for(auto& moved_actor : this_level->inserted_actors){

            this_level->EnumerateActorsAnew();
            for(ufo::Actor::MovedActor& nested_moved_actor : this_level->inserted_actors){
                nested_moved_actor.original_index = nested_moved_actor.actor_to_move->order_index;

                Console::PrintLine("Original index",nested_moved_actor.original_index);

            }

            ufo::Actor* actor_to_move = moved_actor.original_parent->actors[moved_actor.original_index].get();

            multiple_actor_change->changes.push_back(std::make_unique<ufo::ActorChange_Move>(
                this,
                actor_to_move->editor_id,
                moved_actor.original_parent->editor_id,
                moved_actor.original_index,
                moved_actor.move_to_parent->editor_id,
                moved_actor.move_to_index)
            );

            Console::PrintLine(moved_actor.original_parent->editor_name, moved_actor.original_index,
                moved_actor.move_to_parent->editor_name, moved_actor.move_to_index);

            actor_to_move->parent = moved_actor.move_to_parent;

            MoveActor(moved_actor);

        }

        if(!this_level->moving_actor_with_undo_and_redo) this_level->level_changes.push_back(std::move(multiple_actor_change));

    }

    this_level->moving_actor_with_undo_and_redo = false;

    this_level->EnumerateActorsAnew();*/

    MoveActors();
    this_level->inserted_actors.clear();

    ImGui::End();

    //ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, 0.0f);
    ImGui::Begin(std::string("LevelViewport###LevelViewport"+std::to_string(id)).c_str(), nullptr);
    //ImGui::PopStyleVar();

    LevelDrawPhase(engine->graphics.get());

    //From here on it should be fine to do all id-related logic.
    // Let's say the upper half of this function is more ideal to just spawn the actors.
    // Don't address them by their editor_id

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

                for(const int& actor_id : selected_actors){
                    ufo::Actor* actor = this_level->actors_with_stable_id.at(actor_id);

                    remove_actor_change->changes.push_back(ufo::ActorChange_RemoveActor(this,actor->editor_id, actor->parent->editor_id));

                    actor->stash = true;

                }

                this_level->level_changes.push_back(std::move(remove_actor_change));

                show_multi_selection_right_click_pop_up_menu = false;

                selected_actors.clear();
            }

            ImGui::EndPopup();
        }

    }

    if(is_viewport_hovered){

        spawn_cursor->local_position = this_level->active_camera_handles.back()->TransformScreenToWorld(mouse_position_over_screenspace);

        //if(current_tool == Tools::ROTATE){}
        //if(current_tool == Tools::SCALE){}


        if(current_tool == Tools::RESIZE && inspected_actor_id != ufo::Maths::NULL_ID){this_level->actors_with_stable_id.at(inspected_actor_id)->OnResize(editor, this);}

        if(current_tool == Tools::SELECT || current_tool == Tools::MOVE_ACTOR_CLUSTER){
            SelectionUpdate();
        }
        if(current_tool == Tools::PLACE){
            PlaceActors();
        }
    }

    this_level->UpdatePhase(_delta_time);

    for(int actor_id : selected_actors){

        ufo::Actor* _actor = this_level->actors_with_stable_id.at(actor_id);

        const Vector2f pos_min = TranslateToEditorScreenSpace(_actor->GetGlobalPosition()+_actor->editor_hitbox.position);
        const Vector2f pos_max = TranslateToEditorScreenSpace(_actor->GetGlobalPosition()+_actor->editor_hitbox.position+_actor->editor_hitbox.size);

        ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min), UFOEngineStudio::FromVector2fToImVec2(pos_max), 0xFFFFFFFF);
    }

    ImGui::End();

    ImGui::Begin(std::string(currently_viewed_properties_actor_name+"###Properties"+std::to_string(id)).c_str());
    if(selected_actors.size() > 0){
        //-1 is just a temporary index here to test things out, I don't think that value is actually used to anything.
        inspected_actor_id = this_level->actors_with_stable_id.at(selected_actors[0])->editor_id;
    }
    else{
        inspected_actor_id = actor_dedicated_to_viewport_id;
    }

    if(inspected_actor_id != ufo::Maths::NULL_ID){
        ufo::Actor* act_inspected_actor = this_level->actors_with_stable_id.at(inspected_actor_id);

        bool search_field_active = false;

        ImGui::Text("%s",std::string(act_inspected_actor->editor_name+" "+"("+act_inspected_actor->class_name+")").c_str());
        ImGui::Text("%s", std::string("Base-class: "+act_inspected_actor->base_class_name).c_str());
        if(act_inspected_actor->import_mode == ufo::Actor::ImportModes::CUSTOM_CLASS) ImGui::TextWrapped("%s", "Status: Imported actor. You cannot modify the children of this object");
        ImGui::Separator();

        if(search_field_active){
            ImGui::InputText("FindActor...", &act_inspected_actor->find_actor_search_field, ImGuiInputTextFlags_EnterReturnsTrue);
            ufo::Actor* actor = actor = act_inspected_actor->GetActor(act_inspected_actor->find_actor_search_field);

            if(actor) ImGui::Text("Found actor");
        }

        act_inspected_actor->ViewProperties(this, -1);

        //View the editor properties
        for(int i = 0; i < act_inspected_actor->editor_properties.size(); i++){
            act_inspected_actor->editor_properties[i]->Update(this ,act_inspected_actor, act_inspected_actor->editor_name, i);
        }

    }

    ImGui::End();
}


void LevelEditorTab::PlaceInActor(ufo::Actor* _place_inside_actor){

    if(_place_inside_actor){
        ufo::TileMap* tile_map = _place_inside_actor->GetTileMap();

        if(tile_map){

            spawn_cursor->local_position = Vector2f(
                std::floor(spawn_cursor->local_position.x/tile_map->tile_width)*tile_map->tile_width,
                std::floor(spawn_cursor->local_position.y/tile_map->tile_height)*tile_map->tile_height);

            if(spawn_cursor->actors.size()){
                spawn_cursor->actors[0]->local_position = -spawn_cursor->actors[0]->editor_hitbox.position;
            }
        }

        if(ImGui::IsItemClicked(0)){
            if(editor->currently_selected_actor_type != ""){
                if(_place_inside_actor->base_class_name != "ufo::AnimationCluster"){
                    if(editor->spawnable_actor_map.count(editor->currently_selected_actor_type)){
                        auto inst = editor->spawnable_actor_map.at(editor->currently_selected_actor_type)->Spawn(editor);

                        ufo::Actor* inst_ptr = inst.get();

                        if(inst_ptr->base_class_name == "ufo::TileMap" || inst_ptr->base_class_name == "ufo::CollisionGrid" || inst_ptr->base_class_name == "ufo::Level"){
                            inst_ptr->local_position = Vector2f(0.0f, 0.0f);
                        }
                        else{
                            inst_ptr->local_position = spawn_cursor->actors[0]->GetGlobalPosition() - _place_inside_actor->GetGlobalPosition();
                        }

                        //Undo&redo

                        while((int)this_level->level_changes.size()-1 > this_level->current_level_change){
                            Console::PrintLine("loop change stack",this_level->current_level_change, this_level->level_changes.size());
                            this_level->level_changes.pop_back();
                        }

                        this_level->level_changes.push_back(std::make_unique<ufo::ActorChange_AddActor>(this,inst.get()->editor_id, _place_inside_actor->editor_id));

                        this_level->current_level_change++;
                        Console::PrintLine("Actor current change",this_level->current_level_change);

                        _place_inside_actor->AddActorUniquePtr(std::move(inst));



                        actor_dedicated_to_viewport_id = inst_ptr->editor_id;

                        edited_actor_id = ufo::Maths::NULL_ID;

                    }
                }
                else{

                    auto animation_cluster = _place_inside_actor->DynamicCast<ufo::AnimationCluster>();
                    if(animation_cluster){
                        animation_cluster->positions.push_back(spawn_cursor->GetGlobalPosition());
                    }

                }
            }
        }

    }
}

void LevelEditorTab::PlaceActors(){
    if(spawn_cursor->actors.size() == 0){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__, "PlaceActors tool active but spawn_cursor hosts incorrect number of actors:", spawn_cursor->actors.size());
        return;
    }

    if(actor_dedicated_to_viewport_id != ufo::Maths::NULL_ID){
        ufo::Actor* place_inside_actor = nullptr;

        ufo::Actor* act_actor_dedicated_to_viewport = this_level->actors_with_stable_id.at(actor_dedicated_to_viewport_id);

        if(act_actor_dedicated_to_viewport->parent) place_inside_actor = act_actor_dedicated_to_viewport->parent;
        else place_inside_actor = act_actor_dedicated_to_viewport;

        PlaceInActor(place_inside_actor);
    }

    if(edited_actor_id != ufo::Maths::NULL_ID){
        ufo::Actor* place_inside_actor = this_level->actors_with_stable_id.at(edited_actor_id);
        PlaceInActor(place_inside_actor);
    }

}

void LevelEditorTab::MoveActors(){
    int move_from_id = -1;
    int move_to_id = -1;
    int from_index = -1;
    int to_index = -1;
    int number_of_moved_actors = 0;

    if(this_level->inserted_actors.size() == 0) return;

    move_from_id = this_level->inserted_actors[0].original_parent->editor_id;
    move_to_id = this_level->inserted_actors[0].move_to_parent->editor_id;
    to_index = this_level->inserted_actors[0].move_to_index;
    from_index = this_level->inserted_actors[0].original_index;
    number_of_moved_actors = this_level->inserted_actors.size();

    std::vector<std::unique_ptr<ufo::Actor>> new_actors;

    ufo::Actor* move_to_actor = this_level->actors_with_stable_id.at(move_to_id);
    ufo::Actor* move_from_actor = this_level->actors_with_stable_id.at(move_from_id);

    if(!this_level->moving_actor_with_undo_and_redo){
        this_level->RemoveFutureChanges();
        std::unique_ptr multiple_actor_change = std::make_unique<ufo::ActorChange_MultipleActorChange>(false);
        for(auto& moved_actor : this_level->inserted_actors){
            int calculated_to_index = to_index;
            int calculated_from_index = from_index;
            /*if(move_to_id == move_from_id){
                if(to_index > from_index) calculated_to_index -= number_of_moved_actors;
                else calculated_from_index+=number_of_moved_actors;
            }*/

            multiple_actor_change->changes.push_back(
                std::make_unique<ufo::ActorChange_Move>(
                    this,
                    moved_actor.actor_to_move->editor_id,
                    moved_actor.original_parent->editor_id,
                    calculated_from_index,
                    moved_actor.move_to_parent->editor_id,
                    calculated_to_index,
                    to_index-from_index,
                    number_of_moved_actors
                )
            );
        }

        this_level->level_changes.push_back(std::move(multiple_actor_change));
    }

    std::vector<std::unique_ptr<ufo::Actor>> extracting_moved_actors;

    for(int a = from_index; a < from_index+number_of_moved_actors; a++){
        if( move_from_actor->actors[a] == nullptr) continue;
        extracting_moved_actors.push_back(std::move(move_from_actor->actors[a]));
    }

    for(int a = 0; a < to_index; a++){
        if( move_to_actor->actors[a] == nullptr) continue;
        new_actors.push_back(std::move(move_to_actor->actors[a]));
    }

    for(auto&& actor : extracting_moved_actors){
        if(actor == nullptr) continue;
        new_actors.push_back(std::move(actor));
    }

    for(int a = 0; a < move_to_actor->actors.size(); a++){
        if( move_to_actor->actors[a] == nullptr) continue;
        new_actors.push_back(std::move(move_to_actor->actors[a]));
    }

    move_to_actor->actors.clear();

    ufo::PurgeNullPointers(move_from_actor->actors);

    for(auto&& actor : new_actors){
        actor->parent = move_to_actor;
        move_to_actor->actors.push_back(std::move(actor));

    }

    this_level->moving_actor_with_undo_and_redo = false;

}

void LevelEditorTab::SelectionUpdate(){

    if(engine->mouse.is_right_button_pressed){
        for(const int& actor_id : selected_actors){

            ufo::Actor* actor = this_level->actors_with_stable_id.at(actor_id);

            ufo::Actor* act = actor->OnGetFocusedActor(mouse_position_over_screenspace);

            if(act){
                show_multi_selection_right_click_pop_up_menu = true;
            }
        }
    }

    if(engine->mouse.is_left_button_pressed){

        ufo::Actor* act = this_level->GetFocusedActor(mouse_position_over_screenspace);

        if(act){
            bool is_in_selected_actors = false;
            for(const auto& actor_id : selected_actors){
                if(actor_id == act->editor_id) is_in_selected_actors = true;
            }
            if(!is_in_selected_actors){
                selected_actors.clear();
                selected_actors.push_back(act->editor_id);
            }
        }

        rectangle_selection_tool_start_position = engine->mouse.position;


        for(const auto& actor_id : selected_actors){

            ufo::Actor* actor = this_level->actors_with_stable_id.at(actor_id);

            ufo::Actor* focused_actor = actor->OnGetFocusedActor(mouse_position_over_screenspace);

            if(focused_actor){
                if(focused_actor->IsMovable()){
                    current_tool = Tools::MOVE_ACTOR_CLUSTER;

                    this_level->RemoveFutureChanges();

                    std::unique_ptr<ufo::ActorChange_MultipleActorChange> multiple_actor_change = std::make_unique<ufo::ActorChange_MultipleActorChange>(false);

                    for(const auto& inner_actor_id : selected_actors){

                        ufo::Actor* inner_actor = this_level->actors_with_stable_id.at(inner_actor_id);

                        multiple_actor_change->changes.push_back(
                            std::make_unique<ufo::ActorChange_CustomVariableVector2fHandle>(
                                this,
                                inner_actor_id,
                                "local_position",
                                inner_actor->local_position,Vector2f(0.0f, 0.0f))
                        );
                    }

                    this_level->level_changes.push_back(std::move(multiple_actor_change));

                    break;
                }
            }
        }
    }

    if(inspected_actor_id != ufo::Maths::NULL_ID) if(!this_level->actors_with_stable_id.at(inspected_actor_id)->IsMovable()){
        return;
    }

    if(current_tool == Tools::MOVE_ACTOR_CLUSTER){
        Vector2f dp = this_level->active_camera_handles.back()->TransformScreenToWorld(mouse_position_over_screenspace) -
            this_level->active_camera_handles.back()->TransformScreenToWorld(former_mouse_position_over_screenspace);

        for(const auto& actor_id : selected_actors){

            ufo::Actor* actor = this_level->actors_with_stable_id.at(actor_id);

            actor->local_position += dp;

        }
    }

    if(current_tool == Tools::MOVE_ACTOR_CLUSTER && engine->mouse.is_left_button_released){
        for(const int& actor_id : selected_actors){
            ufo::Actor* actor = this_level->actors_with_stable_id.at(actor_id);

            auto local_tile_map = actor->GetTileMap();

            if(local_tile_map){

                actor->local_position = Vector2f(
                    std::floor(actor->local_position.x/local_tile_map->tile_width)*local_tile_map->tile_width,
                    std::floor(actor->local_position.y/local_tile_map->tile_height)*local_tile_map->tile_height) - actor->editor_hitbox.position;
            }
        }

        ufo::ActorChange_MultipleActorChange* movement_change = dynamic_cast<ufo::ActorChange_MultipleActorChange*>(this_level->level_changes.back().get());
        if(movement_change){
            for(size_t a = 0; a < movement_change->changes.size(); a++){
                ufo::ActorChange_CustomVariableVector2fHandle* local_position_change = dynamic_cast<ufo::ActorChange_CustomVariableVector2fHandle*>(movement_change->changes[a].get());

                if(!local_position_change){
                    Console::PrintLine("[UFO-Engine Studio] LevelEditorTab::OnActive: Error converting from ufo::ActorChange to ActorChange_CustomVariableVector2fHandle");
                    throw;
                }

                local_position_change->current_value = this_level->actors_with_stable_id.at(selected_actors[a]) -> local_position;
            }
        }else{
            Console::PrintLine("[UFO-Engine Studio] LevelEditorTab::OnActive: Error converting from ufo::ActorChange to ActorChange_MultipleActorChange");
            throw;
        }

        Console::PrintLine("Set tool Tools::MULTI_SELECT");
        current_tool = Tools::SELECT;

    }

    if(current_tool == Tools::MOVE_ACTOR_CLUSTER) return;

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

        selected_actors.clear();
        this_level->GetSelectedActors(selected_actors, selection_rectangle_world_space);

        ImGui::GetWindowDrawList()->AddRectFilled(
            FromVector2fToImVec2((selected_rectangle.position+FromImVec2ToVector2f(ImGui::GetMainViewport()->Pos))),
            FromVector2fToImVec2((selected_rectangle.position+selected_rectangle.size+FromImVec2ToVector2f(ImGui::GetMainViewport()->Pos))), 0x55555555);
    }

}

void LevelEditorTab::OnMakeDockSpace(ImGuiID _local_dockspace_id,[[maybe_unused]] Editor* _program_state){
    ImGuiDockSpaceSplit(
        _local_dockspace_id,
        ImGui::GetWindowSize(),
        std::string(name_and_imgui_id.c_str()+std::to_string(id)),
        std::string("ContentBrowser###ContentBrowser"+std::to_string(id)),
        SplitDirections::HORIZONTAL, true);
}

void LevelEditorTab::LevelDrawPhase(ufo::Graphics* _graphics){
    //Binding custom buffer to draw stuff in editor level viewport
    _graphics->BindFrameBuffer();

    //I think this part is leaking memory somehow
    _graphics->RescaleFrameBuffer(ImGui::GetWindowSize().x,ImGui::GetWindowSize().y);

    glClearColor(0.0125f, 0.025f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    this_level->DrawPhase(_graphics,ImGui::GetWindowSize().x,ImGui::GetWindowSize().y);
    spawn_cursor->Draw(_graphics, this_level->active_camera_handles.back());
    this_level->DrawGizmosPhase(editor->engine->graphics.get(), this);

    _graphics->UnbindFrameBuffer();
}

void LevelEditorTab::OnSave(Editor* _editor){

    if(!is_new_file){
        auto level_json = this_level->GetAsJson(&gc);
        level_json->Write(_editor->opened_directory_path+"/"+path);
    }
    else{
        std::string global_file_location = std::string(_editor->opened_directory_path+"/"+path);
        Console::PrintLine("suggested_path =",global_file_location);

        SDL_ShowSaveFileDialog(&OnNewActorFile , this, _editor->engine->window, nullptr, 0, global_file_location.c_str());
    }

    Refresh();
}

}
