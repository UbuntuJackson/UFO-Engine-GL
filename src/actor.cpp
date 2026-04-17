#include <sstream>
#include <unordered_map>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "level.h"
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../tilemap/tile_map.h"
#include "../utils/console.h"
#include "sprite.h"
#include "../imgui/imgui.h"
#include "../utils/console.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include <garbage_collector.h>
#include <gc_json.h>
#include "../ufo_garbage_collector/object.h"
#include "../ufo_engine_studio/editor.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/im_vec.h"
#include "actor_undo_and_redo.h"
#include "editor_property.h"
#include "../ufo_engine_studio/advanced_actor_spawner.h"
#include "../utils/conversion.h"

namespace ufo{

Actor::Actor(Vector2f _local_position) : local_position{_local_position}, former_local_position(_local_position){
    editor_id = editor_id_counter++;
    editor_name = "@Instance"+class_name+std::to_string(editor_id);

}

std::string Actor::GetInfo(){
    return "[Instance of type "+class_name+"] "+"name: "+editor_name+", address:"+ufo::MemoryAddressToString(this);
}

Vector2f Actor::GetGlobalPosition(){
    if(parent == nullptr){
        return local_position;
    }
    return local_position + parent->GetGlobalPosition();
}

ufo::Rectangle Actor::GetRectangle(){
    return ufo::Rectangle(GetGlobalPosition(), Vector2f(16.0f, 16.0f));
}

Actor* Actor::GetActor(std::string _path){

    size_t first_of_index = _path.find_first_of("/");
    std::string search_in_actor = _path.substr(0,first_of_index);
    std::string remaining_path = _path.substr(first_of_index+1, _path.size());

    for(const auto& actor : actors){
        if(first_of_index == _path.npos){

            if(_path == actor->editor_name){
                return actor.get();

            }
        }
        else if(search_in_actor == actor->editor_name) return actor->GetActor(remaining_path);
    }

    throw std::runtime_error("[UFO-Engine] Actor::GetActor: Did not find component at "+ _path);

    return nullptr;
}

void Actor::AddNewActors(){

    bool queue_was_empty = new_actor_queue.size() == 0;

    std::vector<Actor*> newly_added_actors;
    newly_added_actors.reserve(new_actor_queue.size());

    for(auto&& actor : new_actor_queue){
        auto actor_ptr = actor.get();

        actor->level = level; // Will be overwritten if *this* is of type Level.
        OnAddActor(actor.get());
        actor->engine = engine;
        newly_added_actors.push_back(actor_ptr);
        actors.push_back(std::move(actor));

    }

    for(auto&& actor : actors){
        actor->AddNewActors();
    }

    for(const auto& actor : newly_added_actors){
        actor->OnSpawn();
    }

    new_actor_queue.clear();

    if(!queue_was_empty && !should_be_sorted){

        for(int i = 0; i < actors.size(); i++){
            actors[i]->order_index = i;
        }
    }
}

void Actor::MarkAllDead(){
    for(auto& actor : actors){
        actor->is_dead = true;
        Console::PrintLine("Deleted", actor.get());
    }
}

void Actor::CleanUpDeadActors(){

    for(int i = actors.size()-1; i != -1; i--){
        if(actors[i]->is_dead){
            actors[i]->MarkAllDead();
            Console::PrintLine("Deleted", actors[i].get());
            actors.erase(actors.begin()+i);
        }
        else{
            actors[i]->CleanUpDeadActors();
        }
    }

}

void Actor::StashActors(){

    for(int i = actors.size()-1; i != -1; i--){
        if(actors[i]->stash){
            Console::PrintLine("Stashed", actors[i]->editing_name);
            level->stashed_actors.push_back(std::move(actors[i]));
            actors.erase(actors.begin()+i);
        }
        else{
            actors[i]->StashActors();
        }
    }

}

void Actor::OnSpawn(){

}

void Actor::OnAddActor(Actor* _actor){}

void Actor::ReplaceActors(UFOEngineStudio::Editor* _editor){
    /*for(int i = 0; i < actors.size(); i++){
        actors[i]->ReplaceActors(_editor);

        if(actors[i]->to_replace){
            actors.at(i) = _editor->replace_with_actor;
        }
    }*/
}

void Actor::Update(float _delta_time){
    former_rectangle = GetRectangle();

    for(const auto& actor : actors){
        actor->Update(_delta_time);
    }

    OnUpdate(_delta_time);

    if(should_be_sorted){
        SortActors();
        Console::PrintLine("Sorting Actors");
    }
}

void Actor::Pause(float _delta_time){

    for(const auto& actor : actors){
        actor->Pause(_delta_time);
    }

    OnPause(_delta_time);

    if(should_be_sorted){
        SortActors();
        Console::PrintLine("Sorting Actors");
    }
}

void Actor::IrregularUpdate(){
    for(const auto& actor : actors){
        actor->IrregularUpdate();
    }
    OnIrregularUpdate();
}

void Actor::OnIrregularUpdate(){

}

void Actor::WidgetDraw(ufo::Graphics* _graphics){
    for(const auto& actor : actors){
        actor->WidgetDraw(_graphics);
    }

    OnWidgetDraw(_graphics);
}

void Actor::OnWidgetDraw(ufo::Graphics* _graphics){

}

void Actor::Draw(ufo::Graphics* _graphics, Camera* _camera){
    OnDraw(_graphics, _camera);
    for(const auto& actor : actors){
        actor->Draw(_graphics, _camera);
    }
}

void Actor::OnDraw(ufo::Graphics* _graphics, Camera* _camera){

}

void Actor::OnInvokeGarbageCollector(){

}

void Actor::InvokeGarbageCollector(){
    OnInvokeGarbageCollector();
    for(const auto& actor : actors){
        actor->InvokeGarbageCollector();
    }
}

// UFO-Engine Studio

void Actor::SetVector2fUndoAndRedo(Vector2f* _ptr, Vector2f _value){
    Vector2f former_value = *_ptr;

    *_ptr = _value;

    level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableVector2fHandle>(_ptr, former_value, _value));
}

void Actor::AddToLevelEditorTabIfSelected(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    int index = 0;

    for(const auto& actor : actors){
        actor->AddToLevelEditorTabIfSelected(_level_editor_tab,index);
        index++;
    }
    if(is_selected) _level_editor_tab->drag_dropped_actors.push_back(DraggedActorWhereAbouts{parent, _index});
}

void Actor::ResetSelectionStatus(){
    if(!should_be_selected) is_selected = false;
    should_be_selected = false;

    for(const auto& actor : actors){
        actor->ResetSelectionStatus();
    }
}

void Actor::InsertActors(){
    for(auto&& inserted_actor : inserted_actor_queue){
        actors.insert(actors.begin()+inserted_actor.index, std::move(inserted_actor.actor));
    }

    for(int i = 0; i < actors.size(); i++){
        actors[i]->order_index = i;
    }

    inserted_actor_queue.clear();

    for(const auto& actor : actors){
        actor->InsertActors();
    }
}

void Actor::InsertActorUniquePtr(std::unique_ptr<Actor>&& _ptr, const int _index){
    _ptr->parent = this;

    //_ptr->order_index = _index;

    inserted_actor_queue.push_back(InsertedActor{_index,std::move(_ptr)});
}

void Actor::SetOrderIndex(int _index){
    if(parent) parent->should_be_sorted = true;
    order_index = _index;
}

void Actor::SortActors(){
    std::sort(actors.begin(), actors.end(), [this](const auto& _a, const auto& _b){
        return _a->order_index < _b->order_index;
    });

    //Not sure why I'm doing it like this. I don't remember
    for(int i = 0; i < actors.size(); i++){
        actors[i]->order_index = i;
    }

    should_be_sorted = false;
}

void Actor::DeclareImportedRecursive(){
    is_imported = true;
    for(auto& actor : new_actor_queue){
        actor->DeclareImportedRecursive();
    }
}

void Actor::UpdateActorStructure(UFOEngineStudio::Editor* _editor, bool _parent_is_modifiable){
    if(import_mode == ImportModes::WRAPPED){

        actors.clear();
        auto act = _editor->spawnable_actor_map.at(class_name)->Spawn(_editor);

        //An issue here: the attributes aren't copied.
        // An idea would be
        // 1.) Save the old actor
        // 2.) Replace the old one with the new one
        // 3.) Move all the custom properties to the new one from the saved old one
        // 4.) Refresh, remove outdated properties, add new ones with Actor::RemoveAndAddEditorPropertiesDuringRuntime

        for(auto&& actor : act->new_actor_queue){
            AddActorUniquePtr(std::move(actor));
        }
        for(auto&& actor : new_actor_queue){
            actor->UpdateActorStructure(_editor, false);
        }
    } else if(import_mode == ImportModes::UNWRAPPED) {
        for(const auto& actor : actors){
            actor->UpdateActorStructure(_editor, false);
        }
    }

}

void Actor::TurnOnEditMode(){
    editing_name = true;
    old_editor_name = editor_name;
}

void Actor::OnUpdateEditorTree(int _index){

}

std::string Actor::GetImportStatus(){
    if(import_mode == ImportModes::MODIFIABLE){
        return "Modifiable";
    }
    if(import_mode == ImportModes::WRAPPED){
        return "Wrapped";
    }

    return "";

}

void Actor::OnAdditionalButtonsForTreeItem(){

}

void Actor::UpdateEditorTree(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    //if(ImGui::GetMousePos().y > _index * ImGui::GetStyle().ItemSpacing.y * )

    bool button_pressed = ImGui::InvisibleButton(std::string("###InvisibleButton"+editor_name+std::to_string(_index)).c_str(),ImVec2(100,3));

    if(ImGui::BeginDragDropTarget()){

        //This code supports drag-dropping multiple actors

        const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("ActorDragDrop");
        if(payload_data && !parent->is_selected){
            if(_editor->active_tab){
                UFOEngineStudio::LevelEditorTab* level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(_editor->active_tab);

                for(const auto& dragged_actor_where_abouts_ : level_editor_tab->drag_dropped_actors){

                    Actor* drag_dropped_actor = dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index].get();

                    while((int)level->level_changes.size()-1 > level->current_level_change){

                        level->level_changes.pop_back();
                    }

                    level->level_changes.push_back(std::make_unique<ufo::ActorChange_Move>(drag_dropped_actor, drag_dropped_actor->parent, drag_dropped_actor->order_index, parent, dragged_actor_where_abouts_.index));
                    level->current_level_change++;

                    drag_dropped_actor->parent = parent;

                    parent->inserted_actor_queue.push_back(InsertedActor{_index,std::move(dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index])});


                }

                std::sort(level_editor_tab->drag_dropped_actors.begin(), level_editor_tab->drag_dropped_actors.end(), [](DraggedActorWhereAbouts& _first, DraggedActorWhereAbouts& _second){
                   return _second.index < _first.index;
                });

                for(const auto& dragged_actor_where_abouts_ : level_editor_tab->drag_dropped_actors){
                    dragged_actor_where_abouts_.parent->actors.erase(dragged_actor_where_abouts_.parent->actors.begin()+dragged_actor_where_abouts_.index);
                }

                level_editor_tab->drag_dropped_actors.clear();
            }
        }

        ImGui::EndDragDropTarget();
    }

    std::string imported_or_not_str = (import_mode != ImportModes::WRAPPED) ? "" : "(.ason)";

    std::string unique_id_actor = editing_name ?
        std::string("###Actor"+std::to_string(editor_id)).c_str() :
        std::string(editor_name+/*" "+std::to_string(order_index)+*/ " ("+class_name+") "+imported_or_not_str+"###Actor"+std::to_string(editor_id)).c_str();

    bool tree_node_opened = ImGui::TreeNodeEx(std::string("###ActorTree"+std::to_string(editor_id)).c_str(), ImGuiTreeNodeFlags_SpanTextWidth);

    ImGui::SameLine();

    if(editing_name){
        ImGui::SameLine();
        ImGui::InputText(("###EditText"+std::to_string(editor_id)).c_str(),&editor_name);

        if((ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()) || ImGui::IsKeyPressed(ImGuiKey_Enter)){
            editing_name = false;
        }
    }
    else{
        bool selectable_text = ImGui::Selectable(unique_id_actor.c_str(),&is_selected, ImGuiSelectableFlags_None);

        if(selectable_text){

            if(!ImGui::IsKeyDown(ImGuiKey_LeftShift)){
                if(_editor->active_tab){
                    UFOEngineStudio::LevelEditorTab* level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(_editor->active_tab);
                    level_editor_tab->reset_selection_status = true;
                    should_be_selected = is_selected;

                }

            }
            _editor->set_all_actors_properties_open_to_false = true;
            should_open_properties = true;

            if(_level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::EDIT_TILEMAP && base_class_name != "ufo::TileMap"){
                _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::SELECT;

            }
        }

    }

    if(ImGui::BeginPopupContextItem(("Options###Options"+std::to_string(editor_id)).c_str())){
        if(ImGui::MenuItem("Rename")){
            TurnOnEditMode();
        }
        if(!is_top_actor_in_editor && !unremovable){
            if(ImGui::MenuItem("Delete")){
                level->RemoveFutureChanges();

                level->level_changes.push_back(std::make_unique<ufo::ActorChange_RemoveActor>(this));

                stash = true;

            }
        }
        if(ImGui::MenuItem("Add Actor")){
            adding_new_actor = true;

        }
        if(ImGui::MenuItem("Chance Actor Type")){

        }
        ImGui::EndPopup();
    }

    /*if(changing_actor_type){
        ImGui::Begin("Changing Actor Type");
        for(const auto& [k,v] : _editor->spawnable_actor_map){
            if(ImGui::Button(std::string("Make "+k).c_str())){
                auto inst = v->Spawn(_editor);
                inst->class_name = k;
                inst->base_class_name = v->base;
                AddActorUniquePtr(std::move(inst));
                adding_new_actor = false;
            }
        }
        if(ImGui::Button("Cancel")){
            changing_actor_type = false;
        }
        ImGui::End();
    }*/

    if(adding_new_actor){
        //Read from json somehow to add the attributes, however tf that is gonna happen

        ImGui::Begin("Adding Actor");

        std::map<std::string, std::vector<UFOEngineStudio::AdvancedActorSpawner*>> categories;

        for(const auto& [k,v] : _editor->spawnable_actor_map){
            if(!categories.count(v->category)) categories.emplace(v->category, std::vector<UFOEngineStudio::AdvancedActorSpawner*>{});
            categories.at(v->category).push_back(v.get());
        }

        for(const auto& [k,v] : categories){
            ImGui::Text("%s",k.c_str());
            for(const auto& s : v){
                if(ImGui::Button(std::string("Add "+s->class_name+"###Add"+k+s->class_name).c_str())){
                    auto inst = s->Spawn(_editor);

                    level->RemoveFutureChanges();

                    level->level_changes.push_back(std::make_unique<ufo::ActorChange_AddActor>(inst.get()));

                    AddActorUniquePtr(std::move(inst));
                    adding_new_actor = false;
                }
            }
        }

        ImGui::Separator();

        for(const auto& [k,v] : _editor->spawnable_actor_map){
            if(ImGui::Button(std::string("Add "+k).c_str())){
                auto inst = v->Spawn(_editor);

                AddActorUniquePtr(std::move(inst));
                adding_new_actor = false;
            }
        }
        if(ImGui::Button("Cancel")){
            adding_new_actor = false;
        }
        ImGui::End();
    }

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
        dragged_actor_where_abouts = DraggedActorWhereAbouts{parent, _index};

        ImGui::SetDragDropPayload("ActorDragDrop", &dragged_actor_where_abouts, sizeof(DraggedActorWhereAbouts));
        ImGui::Text("%s",editor_name.c_str());
        is_selected = true;

        if(_editor->active_tab){
            UFOEngineStudio::LevelEditorTab* level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(_editor->active_tab);
            level_editor_tab->drag_dropped_actors.clear();
            level->AddToLevelEditorTabIfSelected(level_editor_tab, 0);
        }

        ImGui::EndDragDropSource();
    }

    if(ImGui::BeginDragDropTarget()){

        /*const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("ActorDragDrop");
        if(payload_data){
            DraggedActorWhereAbouts* dragged_actor_where_abouts_ = (DraggedActorWhereAbouts*)(payload_data->Data);

            dragged_actor_where_abouts_->parent->actors[dragged_actor_where_abouts_->index]->parent = this;

            new_actor_queue.push_back(std::move(dragged_actor_where_abouts_->parent->actors[dragged_actor_where_abouts_->index]));

            dragged_actor_where_abouts_->parent->actors.erase(dragged_actor_where_abouts_->parent->actors.begin()+dragged_actor_where_abouts_->index);

        }*/

        const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("ActorDragDrop");
        if(payload_data && !is_selected){
            if(_editor->active_tab){
                UFOEngineStudio::LevelEditorTab* level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(_editor->active_tab);

                for(const auto& dragged_actor_where_abouts_ : level_editor_tab->drag_dropped_actors){

                    Actor* drag_dropped_actor = dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index].get();

                    while((int)level->level_changes.size()-1 > level->current_level_change){

                        level->level_changes.pop_back();
                    }

                    level->level_changes.push_back(std::make_unique<ufo::ActorChange_Move>(drag_dropped_actor, drag_dropped_actor->parent, drag_dropped_actor->order_index, this, dragged_actor_where_abouts_.index));
                    level->current_level_change++;

                    drag_dropped_actor->parent = this;

                    new_actor_queue.push_back(std::move(dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index]));
                }

                std::sort(level_editor_tab->drag_dropped_actors.begin(), level_editor_tab->drag_dropped_actors.end(), [](DraggedActorWhereAbouts& _first, DraggedActorWhereAbouts& _second){
                   return _second.index < _first.index;
                });

                for(const auto& dragged_actor_where_abouts_ : level_editor_tab->drag_dropped_actors){

                    dragged_actor_where_abouts_.parent->actors.erase(dragged_actor_where_abouts_.parent->actors.begin()+dragged_actor_where_abouts_.index);

                }

                level_editor_tab->drag_dropped_actors.clear();
            }

        }

        ImGui::EndDragDropTarget();
    }


    OnAdditionalButtonsForTreeItem();

    /*if(ImGui::IsItemHovered()){
        Console::PrintLine("ImGui::GetItemRectMin.y", ImGui::GetItemRectMin().y);
    }*/

    if(tree_node_opened){

        OnUpdateEditorTree(_index);

        for(int i = 0; i < actors.size(); i++){

            actors[i]->UpdateEditorTree(_editor,_level_editor_tab,i);

        }

        ImGui::TreePop();
    }
}

void Actor::OpenProperties(){
    properties_open = false;
    if(should_open_properties){
        properties_open = true;
        should_open_properties = false;
    }

    for(const auto& actor : actors){
        actor->OpenProperties();
    }

}

void Actor::GetSelectedActors(std::vector<Actor*>& _selected_actors, ufo::Rectangle _selection_rectangle_world_space){
    if(!ImGui::IsKeyDown(ImGuiKey_LeftShift)) is_selected_in_viewport = false;

    if(
        ufo::Maths::RectangleVsPoint(_selection_rectangle_world_space , GetGlobalPosition())
        && editor_name != "ControllableCamera (Editor Tool)" && editor_name != "SpawnCursor (Editor Tool)"
    ){

        _selected_actors.push_back(this);
        return;
        //Console::PrintLine(editor_name,class_name);
    }

    for(const auto& actor : actors){
        actor->GetSelectedActors(_selected_actors, _selection_rectangle_world_space);
    }
}

void Actor::GetPreviouslySelectedActors(std::vector<Actor*>& _selected_actors, ufo::Rectangle _selection_rectangle_world_space){

    if(
        editor_name != "ControllableCamera (Editor Tool)" && editor_name != "SpawnCursor (Editor Tool)"
    ){
        if(is_selected_in_viewport){
            _selected_actors.push_back(this);
            return;
        }
    }

    for(const auto& actor : actors){
        actor->GetPreviouslySelectedActors(_selected_actors, _selection_rectangle_world_space);
    }
}
void Actor::SetActorsUnselectedInViewport(){
    is_selected_in_viewport = false;

    for(const auto& actor : actors){
        actor->SetActorsUnselectedInViewport();
    }
}

//Compare with .ason if there is one for the class-name and update the components
void Actor::RemoveAndAddEditorPropertiesDuringRuntime(UFOEngineStudio::Editor* _editor){
    if(_editor->spawnable_actor_map.count(class_name)){

        //Basically, get the actor that would be spawned in the editor, and update according to that
        UFOEngineStudio::AdvancedActorSpawner* advanced_spawner_of_this_class = _editor->spawnable_actor_map.at(class_name).get();

        std::map<std::string, std::unique_ptr<ufo::EditorProperty>> properties_template;

        std::map<std::string, ufo::EditorProperty*> properties_of_this;

        for(const auto& property : advanced_spawner_of_this_class->custom_properties){
             properties_template.emplace(property->variable_name,property->Copy());
        }

        for(const auto& property : editor_properties){
            properties_of_this.emplace(property->variable_name, property.get());
        }

        for(const auto& [k,v] : properties_template){

            //Does custom property exist in this actor? If not, then add it
            if(!properties_of_this.count(k)){
                editor_properties.push_back(v->Copy());
            }
            else{
                //Update alias. This is a bit weird since I remember making it so alias is updated anyway.
                // could try remove this and see what happens
                properties_of_this.at(k)->alias = properties_template.at(k)->alias;
            }
        }

        for(const auto& [k,v] : properties_of_this){
            if(!properties_template.count(k)){
                v->to_be_removed = true;
            }
        }

        for(int i = editor_properties.size()-1; i != -1; i--){
            if(editor_properties[i]->to_be_removed){
                editor_properties.erase(editor_properties.begin()+i);
            }
        }
    }

    for(const auto& actor : actors){
        actor->RemoveAndAddEditorPropertiesDuringRuntime(_editor);
    }
}

bool Actor::InputFloatWithUndoAndRedo(const std::string& _id, float* _ptr){
    float value_before_edit = *_ptr;
    if(ImGui::InputFloat(_id.c_str(), _ptr)){

        level->RemoveFutureChanges();

        level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableFloatHandle>(_ptr, value_before_edit, *_ptr));

        return true;
    }

    return false;
}

//This function is currently untested
bool Actor::InputIntWithUndoAndRedo(const std::string& _id, int* _ptr){
    int value_before_edit = *_ptr;
    if(ImGui::InputInt(_id.c_str(), _ptr)){

        level->RemoveFutureChanges();

        level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableIntHandle>(_ptr, value_before_edit, *_ptr));

        return true;
    }

    return false;
}

//This function is currently untested
bool Actor::InputTextWithUndoAndRedo(const std::string& _id, std::string* _ptr){
    std::string value_before_edit = *_ptr;
    if(ImGui::InputText(_id.c_str(), _ptr)){

        level->RemoveFutureChanges();

        level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableStringHandle>(_ptr, value_before_edit, *_ptr));

        return true;
    }

    return false;
}

void Actor::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    bool search_field_active = false;

    ImGui::Text("%s",std::string(editor_name+" "+"("+class_name+")").c_str());
    ImGui::Text("%s", std::string("Base-class: "+base_class_name).c_str());
    if(import_mode == ImportModes::WRAPPED) ImGui::TextWrapped("%s", "Status: Imported actor. You cannot modify the children of this object");
    ImGui::Separator();

    if(search_field_active){
        ImGui::InputText("FindActor...", &find_actor_search_field, ImGuiInputTextFlags_EnterReturnsTrue);
        Actor* actor = nullptr;
        try{
             actor = GetActor(find_actor_search_field);
        }
        catch(const std::exception& _error){

        }

        if(ImGui::Button(std::string("Found actor: "+find_actor_search_field).c_str())){

        }
    }

    InputFloatWithUndoAndRedo(std::string("local_position.x###local_position.x"+editor_name+std::to_string(_index)).c_str(), &local_position.x);
    InputFloatWithUndoAndRedo(std::string("local_position.y###local_position.y"+editor_name+std::to_string(_index)).c_str(), &local_position.y);

    for(int i = 0; i < editor_properties.size(); i++){
        editor_properties[i]->Update(this, editor_name, i);
    }

}

void Actor::ViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    if(properties_open){
        OnViewProperties(_level_editor_tab,_index);
        _level_editor_tab->currently_viewed_properties_actor_name = editor_name;
    }
    for(int i = 0; i < actors.size(); i++){
        actors[i]->ViewProperties(_level_editor_tab,i);
    }
}

void Actor::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    if(is_selected_in_viewport){
        const Vector2f pos_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition())+editor_hitbox.position;
        const Vector2f pos_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition())+editor_hitbox.position+editor_hitbox.size;

        ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min), UFOEngineStudio::FromVector2fToImVec2(pos_max), 0xFFFFFFFF);
    }

    //Adjusting the spawn-cursor to not make usage too disorienting
    if(properties_open){

        auto local_tile_map = IsInTileMap();
        if(local_tile_map){
            _level_editor_tab->spawn_cursor->local_position = Vector2f(
                std::floor(_level_editor_tab->spawn_cursor->local_position.x/local_tile_map->tile_width)*local_tile_map->tile_width,
                std::floor(_level_editor_tab->spawn_cursor->local_position.y/local_tile_map->tile_height)*local_tile_map->tile_height);
        }
    }

    const Vector2f pos_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+editor_hitbox.position);
    const Vector2f pos_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+editor_hitbox.position+editor_hitbox.size);

    if(editor_viewport_text != "") ImGui::GetWindowDrawList()->AddText(ImVec2(pos_max.x, pos_max.y), 0xFFFFFFFF,editor_viewport_text.c_str());

    ImU32 colour = 0xFFFFFFFF;
    if(parent->base_class_name != "ufo::Level") colour = 0xFF664422;

    ImU32 line_clour =  0x66664422;

    Vector2f this_screen_pos = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition());

    /*for(const auto& child : actors){

        Vector2f child_screen_pos = _level_editor_tab->TranslateToEditorScreenSpace(child->GetGlobalPosition());

        ImGui::GetWindowDrawList()->AddLine(ImVec2(this_screen_pos.x, this_screen_pos.y), ImVec2(this_screen_pos.x, child_screen_pos.y), line_clour, 1.0f);
        ImGui::GetWindowDrawList()->AddLine(ImVec2(this_screen_pos.x, child_screen_pos.y), ImVec2(child_screen_pos.x, child_screen_pos.y), line_clour, 1.0f);
        }*/

    ImGui::GetWindowDrawList()->AddLine(ImVec2(this_screen_pos.x, this_screen_pos.y-5.0f), ImVec2(this_screen_pos.x, this_screen_pos.y+5.0f), colour, 1.0f);
    ImGui::GetWindowDrawList()->AddLine(ImVec2(this_screen_pos.x-5.0f, this_screen_pos.y), ImVec2(this_screen_pos.x+5.0f, this_screen_pos.y), colour, 1.0f);

    if(is_selected){
        if(ImGui::IsItemClicked(0) && _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::PLACE){
            if(_editor->currently_selected_actor_type != ""){
                if(_editor->spawnable_actor_map.count(_editor->currently_selected_actor_type)){
                    auto inst = _editor->spawnable_actor_map.at(_editor->currently_selected_actor_type)->Spawn(_editor);

                    if(!IsInTileMap()) inst->local_position = level->active_camera_handles.back()->TransformScreenToWorld(_level_editor_tab->mouse_position_over_screenspace) - GetGlobalPosition();
                    else inst->local_position = _level_editor_tab->spawn_cursor->GetGlobalPosition() - GetGlobalPosition();

                    //Undo&redo

                    level->RemoveFutureChanges();

                    level->level_changes.push_back(std::make_unique<ufo::ActorChange_AddActor>(inst.get()));

                    AddActorUniquePtr(std::move(inst));
                }
            }
        }
    }

}

Actor* Actor::GetInspectedActor(){
    if(import_mode != ImportModes::WRAPPED){
        if(is_selected) return this;

        for(const auto& actor : actors){
            Actor* act = actor->GetInspectedActor();
            if(act) return act;
        }
    }
    return nullptr;
}

void Actor::OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

Actor* Actor::GetFocusedActor(Vector2f _mouse_position_over_screenspace){
    if(import_mode != ImportModes::WRAPPED){
        for(const auto& actor : actors){
            Actor* act = actor->GetFocusedActor(_mouse_position_over_screenspace);
            if(act) return act;
        }
    }

    return OnGetFocusedActor(_mouse_position_over_screenspace);
}

Actor* Actor::OnGetFocusedActor(Vector2f _mouse_position_over_screenspace){
    const Vector2f world_mouse = level->active_camera_handles.back()->TransformScreenToWorld(_mouse_position_over_screenspace);

    if(ufo::Maths::RectangleVsPoint(ufo::Rectangle(GetGlobalPosition()+editor_hitbox.position, editor_hitbox.size), world_mouse)){
        return this;
    }
    return nullptr;
}

void Actor::OnHandleSingleSelect(UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Actor::OnFocused(UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    //Basically grab on click, ungrab on release. This does not make sense if you want to
    // be able to spawn multiple actors in a row by holding the mouse button
    if(!is_grabbed_by_cursor && engine->mouse.is_left_button_pressed){

        is_grabbed_by_cursor = true;

        should_open_properties = true;
        _level_editor_tab->editor->set_all_actors_properties_open_to_false = true;

        level->RemoveFutureChanges();

        level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableVector2fHandle>(&local_position, local_position, Vector2f(0.0f, 0.0f)));

    }

}

bool Actor::OnGrabbedByCursor(Vector2f _mouse_position_over_screenspace, Vector2f _former_mouse_position_over_screenspace){

    Vector2f world_mouse = level->active_camera_handles.back()->TransformScreenToWorld(_mouse_position_over_screenspace);
    Vector2f former_world_mouse = level->active_camera_handles.back()->TransformScreenToWorld(_former_mouse_position_over_screenspace);

    if(is_grabbed_by_cursor){
        Vector2f dp = world_mouse - former_world_mouse;

        local_position += dp;

        if(engine->mouse.is_left_button_released){

            is_grabbed_by_cursor = false;
            TileMap* tile_map = IsInTileMap();
            if(tile_map){
                local_position = Vector2f(
                    std::floor(local_position.x/tile_map->tile_width)*tile_map->tile_width,
                    std::floor(local_position.y/tile_map->tile_height)*tile_map->tile_height);
            }

            //This looks extremely error prone.
            ufo::ActorChange_CustomVariableVector2fHandle* position_change = dynamic_cast<ufo::ActorChange_CustomVariableVector2fHandle*>(level->level_changes.back().get());

            if(position_change){
                position_change->current_value = local_position;

            }
            else{
                Console::PrintLine("[UFO-Engine Studio] Actor::OnUpdateEditorViewportFocus: Undo & Redo action was added while ufo::ActorChange_CustomVariableVector2fHandle* was handled");
                throw;
            }
        }

        return true;
    }

    return false;
}

bool Actor::GrabbedByCursor(Vector2f _mouse_position_over_screenspace, Vector2f _former_mouse_position_over_screenspace){

    for(const auto& actor : actors){
        bool act_is_grabbed_by_cursor = actor->GrabbedByCursor(_mouse_position_over_screenspace, _former_mouse_position_over_screenspace);
        if(act_is_grabbed_by_cursor) return true;
    }

    return OnGrabbedByCursor(_mouse_position_over_screenspace, _former_mouse_position_over_screenspace);

}

void Actor::UpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    if(import_mode != ImportModes::WRAPPED){
        for(const auto& actor : actors){
            actor->UpdateEditorViewport(_editor, _level_editor_tab);

        }
    }

    OnUpdateEditorViewport(_editor, _level_editor_tab);
}

//Might not use
void Actor::OnSelectedInViewport(UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

TileMap* Actor::IsInTileMap(){
    if(parent == nullptr) return nullptr;
    else{
        if(parent->class_name == "ufo::TileMap") return parent->DynamicCast<TileMap>();
        else{
            return parent->IsInTileMap();
        }
    }
    return nullptr;
}

void Actor::DrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    OnDrawGizmos(_graphics, _camera, _level_editor_tab);
    for(const auto& actor : actors){
        actor->DrawGizmos(_graphics, _camera, _level_editor_tab);
    }
}

void Actor::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

ufo::gc::JsonMap* Actor::GetAsJson(ufo::GarbageCollector* _gc){
    ufo::gc::JsonMap* this_actor = _gc->New<ufo::gc::JsonMap>();
    this_actor->map.emplace("is_imported", _gc->New<ufo::gc::JsonNumber>(int(is_imported)));
    this_actor->map.emplace("import_mode", _gc->New<ufo::gc::JsonNumber>(int(import_mode)));
    this_actor->map.emplace("name", _gc->New<ufo::gc::JsonString>(editor_name));
    this_actor->map.emplace("base_class_name", _gc->New<ufo::gc::JsonString>(base_class_name));
    this_actor->map.emplace("class_name", _gc->New<ufo::gc::JsonString>(class_name));
    auto j_custom_editor_properties = _gc->New<ufo::gc::JsonMap>();
    this_actor->map.emplace("custom_editor_properties", j_custom_editor_properties);

    for(const auto& property : editor_properties){

        j_custom_editor_properties->map.emplace(property->variable_name, property->GetJson(_gc));

    }

    this_actor->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(local_position.x));
    this_actor->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(local_position.y));

    ufo::gc::JsonArray* children = _gc->New<ufo::gc::JsonArray>();

    if(import_mode == ImportModes::UNWRAPPED){
        for(const auto& actor : actors){
            if(actor->is_savable) children->array.push_back(actor->GetAsJson(_gc));
        }
    }

    this_actor->map.emplace("actors", children);

    return this_actor;
}

//Actor generator calls this.
void Actor::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){

}

}
