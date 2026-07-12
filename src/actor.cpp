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
#include "../utils/console.h"
#include <garbage_collector.h>
#include <gc_json.h>
#include "../ufo_garbage_collector/object.h"
#include "../utils/conversion.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/advanced_actor_spawner.h"
#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "../ufo_engine_studio/editor.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/utility_objects/new_actor_placeholder.h"
#include "../ufo_engine_studio/im_vec.h"
#include "actor_undo_and_redo.h"
#include "editor_property.h"
#endif //UFO_ENGINE_STUDIO

namespace ufo{

Actor::Actor(Vector2f _local_position) : local_position{_local_position}, former_local_position(_local_position){
    editor_id = editor_id_counter++;
    editor_name = "@Instance"+class_name+std::to_string(editor_id);

#ifdef UFO_ENGINE_STUDIO

    float_handles.emplace("local_position.x",&local_position.x);
    float_handles.emplace("local_position.y",&local_position.y);

    vector2f_handles.emplace("local_position",&local_position);

#endif

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

        //actor->OnAddActor(actor_ptr);

        //All levels are already preset, so if actor_ptr->level is not nullptr, then just let it be.
        if(!actor_ptr->level) actor_ptr->level = level;
#ifdef UFO_ENGINE_STUDIO

        //actor_ptr is guaranteed to be the correct instance of Level
        actor_ptr->level->actors_with_stable_id.emplace(actor_ptr->editor_id, actor_ptr);
#endif

        actor_ptr->engine = engine;
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

        for(int i = 0; i < (int)actors.size(); i++){
            actors[i]->order_index = i;
        }
    }
}

void Actor::MarkAllDead(){
    for(auto& actor : actors){
        actor->is_dead = true;
        Console::PrintLine("Deleted Actor", actor.get());
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

void Actor::InsertActors(){
    for(int a = inserted_actor_queue.size()-1; a != -1; a--){
        auto&& inserted_actor = inserted_actor_queue[a];

        actors.insert(actors.begin()+inserted_actor.index, std::move(inserted_actor.actor));
    }

    for(int i = 0; i < (int)actors.size(); i++){
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
    for(int i = 0; i < (int)actors.size(); i++){
        actors[i]->order_index = i;
    }

    should_be_sorted = false;
}

void Actor::OnSpawn(){

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

void Actor::OnWidgetDraw([[maybe_unused]] ufo::Graphics* _graphics){

}

void Actor::Draw(ufo::Graphics* _graphics, Camera* _camera){
    OnDraw(_graphics, _camera);
    for(const auto& actor : actors){
        actor->Draw(_graphics, _camera);
    }
}

void Actor::OnDraw([[maybe_unused]] ufo::Graphics* _graphics, [[maybe_unused]] Camera* _camera){

}

ufo::Rectangle Actor::GetEditorHitBox(){

    if(class_name == "ufo::Sprite" ||
        class_name == "ufo::Animation" ||
        class_name == "ufo::PlatformerRectangleCollision" ||
        class_name == "ufo::RectangularArea"
    ){
        ufo::Rectangle main_editor_hitbox = editor_hitbox;
        main_editor_hitbox.position+=GetGlobalPosition();
        return editor_hitbox;
    }

    for(const auto& actor : actors){
        if(actor->class_name == "ufo::Sprite" ||
            actor->class_name == "ufo::Animation" ||
            actor->class_name == "ufo::PlatformerRectangleCollision" ||
            actor->class_name == "ufo::RectangularArea"
        ){
            ufo::Rectangle main_editor_hitbox = actor->editor_hitbox;
            main_editor_hitbox.position+=actor->GetGlobalPosition();
            return main_editor_hitbox;
        }
    }

    return editor_hitbox;
}

ufo::gc::JsonMap* Actor::GetAsJson(ufo::GarbageCollector* _gc){

    ufo::gc::JsonMap* this_actor = _gc->New<ufo::gc::JsonMap>();
    this_actor->map.emplace("name", _gc->New<ufo::gc::JsonString>(editor_name));
    this_actor->map.emplace("class_name", _gc->New<ufo::gc::JsonString>(class_name));
    auto j_custom_editor_properties = _gc->New<ufo::gc::JsonMap>();
    this_actor->map.emplace("custom_editor_properties", j_custom_editor_properties);

#ifdef UFO_ENGINE_STUDIO
    if(editor_name == "Main"){
        ufo::Rectangle main_editor_hitbox = GetEditorHitBox();

        auto j_hitbox = _gc->New<ufo::gc::JsonMap>();

        this_actor->map.emplace("editor_hitbox", j_hitbox);
        j_hitbox->map.emplace("x",_gc->New<ufo::gc::JsonNumber>(main_editor_hitbox.position.x));
        j_hitbox->map.emplace("y",_gc->New<ufo::gc::JsonNumber>(main_editor_hitbox.position.y));
        j_hitbox->map.emplace("width",_gc->New<ufo::gc::JsonNumber>(main_editor_hitbox.size.x));
        j_hitbox->map.emplace("height",_gc->New<ufo::gc::JsonNumber>(main_editor_hitbox.size.y));
    }

    for(const auto& property : editor_properties){

        j_custom_editor_properties->map.emplace(property->variable_name, property->GetJson(_gc));

    }
#endif

    this_actor->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(local_position.x));
    this_actor->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(local_position.y));

    ufo::gc::JsonArray* children = _gc->New<ufo::gc::JsonArray>();

#ifdef UFO_ENGINE_STUDIO
    if(import_mode == ImportModes::UNWRAPPED){
        for(const auto& actor : actors){
            if(actor->is_savable) children->array.push_back(actor->GetAsJson(_gc));
        }
        this_actor->map.emplace("actors", children);
    }
#endif

    return this_actor;
}

//Actor generator calls this.
void Actor::OnLoadDefaultProperties([[maybe_unused]] ufo::gc::JsonMap* _json){

}

void Actor::OnInvokeGarbageCollector(){

}

void Actor::InvokeGarbageCollector(){
    OnInvokeGarbageCollector();
    for(const auto& actor : actors){
        actor->InvokeGarbageCollector();
    }
}

TileMap* Actor::GetTileMap(){
    if(class_name == "ufo::TileMap") return this->DynamicCast<TileMap>();
    if(parent == nullptr) return nullptr;
    else{
        if(parent->class_name == "ufo::TileMap") return parent->DynamicCast<TileMap>();
        else{
            return parent->GetTileMap();
        }
    }
    return nullptr;
}

// UFO-Engine Studio
#ifdef UFO_ENGINE_STUDIO

void Actor::ReplaceActors([[maybe_unused]] UFOEngineStudio::Editor* _editor){
    /*for(int i = 0; i < actors.size(); i++){
        actors[i]->ReplaceActors(_editor);

        if(actors[i]->to_replace){
            actors.at(i) = _editor->replace_with_actor;
        }
    }*/
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

void Actor::SetVector2fUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab, std::string _name, Vector2f _value){
    Vector2f former_value = *vector2f_handles.at(_name);

    *vector2f_handles.at(_name) = _value;

    level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableVector2fHandle>(_level_editor_tab,editor_id,_name, former_value, _value));
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

void Actor::UpdateActorStructure(UFOEngineStudio::Editor* _editor, bool _parent_is_modifiable){

    for(int a = 0; a < (int)actors.size(); a++){
        if(actors[a]->import_mode == ImportModes::WRAPPED){
            Console::PrintLine("UpdateActorStructure actors",actors[a]->editor_name);

            auto old_actor = std::move(actors[a]);

            actors[a] = std::move(_editor->spawnable_actor_map.at(old_actor->class_name)->Spawn(_editor));

            actors[a]->local_position = old_actor->local_position;
            actors[a]->parent = old_actor->parent;
            actors[a]->level = old_actor->level;
            actors[a]->engine = old_actor->engine;
            actors[a]->properties_open = old_actor->properties_open;
            actors[a]->editor_name = old_actor->editor_name;
            actors[a]->editor_id = old_actor->editor_id;
            if(old_actor->is_editor_hit_box_unique_per_instance) actors[a]->editor_hitbox = old_actor->editor_hitbox;
            level->actors_with_stable_id.at(actors[a]->editor_id) = actors[a].get();

            actors[a]->OnSpawn();

            actors[a]->editor_properties.clear();
            for(auto&& property : old_actor->editor_properties){
                actors[a]->editor_properties.push_back(property->Copy());
            }

        }

        actors[a]->UpdateActorStructure(_editor, _parent_is_modifiable);
    }

    for(int a = 0; a < (int)new_actor_queue.size(); a++){
        if(new_actor_queue[a]->import_mode == ImportModes::WRAPPED){

            auto old_actor = std::move(new_actor_queue[a]);

            new_actor_queue[a] = std::move(_editor->spawnable_actor_map.at(old_actor->class_name)->Spawn(_editor));

            new_actor_queue[a]->local_position = old_actor->local_position;
            new_actor_queue[a]->parent = old_actor->parent;
            new_actor_queue[a]->level = old_actor->level;
            new_actor_queue[a]->engine = old_actor->engine;
            new_actor_queue[a]->properties_open = old_actor->properties_open;
            new_actor_queue[a]->editor_name = old_actor->editor_name;
            new_actor_queue[a]->editor_id = old_actor->editor_id;
            if(old_actor->is_editor_hit_box_unique_per_instance) actors[a]->editor_hitbox = old_actor->editor_hitbox;
            level->actors_with_stable_id.at(new_actor_queue[a]->editor_id) = new_actor_queue[a].get();

            new_actor_queue[a]->OnSpawn();

            new_actor_queue[a]->editor_properties.clear();
            for(auto&& property : old_actor->editor_properties){
                new_actor_queue[a]->editor_properties.push_back(property->Copy());
            }

        }

        new_actor_queue[a]->UpdateActorStructure(_editor, _parent_is_modifiable);
    }

}

void Actor::TurnOnEditMode(){
    editing_name = true;
    old_editor_name = editor_name;
}

void Actor::OnUpdateEditorTree([[maybe_unused]] int _index){

}

void Actor::OnAdditionalButtonsForTreeItem(){

}

void Actor::UpdateEditorTree(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    bool button_pressed = ImGui::InvisibleButton(std::string("###InvisibleButton"+editor_name+std::to_string(_index)).c_str(),ImVec2(100,3));

    if(ImGui::BeginDragDropTarget()){

        //This code supports drag-dropping multiple actors

        const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("ActorDragDrop");
        if(payload_data && !parent->is_selected){
            if(_editor->active_tab){
                UFOEngineStudio::LevelEditorTab* level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(_editor->active_tab);

                std::unique_ptr multiple_actor_change = std::make_unique<ufo::ActorChange_MultipleActorChange>();

                level->RemoveFutureChanges();

                //Sort from smallest to largest
                std::sort(level_editor_tab->drag_dropped_actors.begin(), level_editor_tab->drag_dropped_actors.end(), [](DraggedActorWhereAbouts& _first, DraggedActorWhereAbouts& _second){
                   return _second.index > _first.index;
                });

                for(int where_abouts_index = level_editor_tab->drag_dropped_actors.size()-1; where_abouts_index != -1; where_abouts_index--){
                    const auto dragged_actor_where_abouts_ = level_editor_tab->drag_dropped_actors[where_abouts_index];

                    Actor* drag_dropped_actor = dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index].get();

                    multiple_actor_change->changes.push_back(std::make_unique<ufo::ActorChange_Move>(
                        _level_editor_tab,drag_dropped_actor->editor_id,
                        drag_dropped_actor->parent->editor_id,
                        drag_dropped_actor->order_index,
                        parent->editor_id,
                        dragged_actor_where_abouts_.index));

                    drag_dropped_actor->parent = parent;

                }

                level->level_changes.push_back(std::move(multiple_actor_change));

                for(int where_abouts_index = 0; where_abouts_index < (int)level_editor_tab->drag_dropped_actors.size(); where_abouts_index++){
                    const auto dragged_actor_where_abouts_ = level_editor_tab->drag_dropped_actors[where_abouts_index];
                    parent->InsertActorUniquePtr(std::move(dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index]), _index);

                }

                for(int where_abouts_index = level_editor_tab->drag_dropped_actors.size()-1; where_abouts_index != -1; where_abouts_index--){

                    const auto dragged_actor_where_abouts_ = level_editor_tab->drag_dropped_actors[where_abouts_index];
                    dragged_actor_where_abouts_.parent->actors.erase(dragged_actor_where_abouts_.parent->actors.begin()+dragged_actor_where_abouts_.index);
                }

                level_editor_tab->drag_dropped_actors.clear();
            }
        }

        ImGui::EndDragDropTarget();
    }

    std::string imported_or_not_str = (import_mode != ImportModes::WRAPPED) ? "" : "(.ason)";

    std::string visible_text = std::string(editor_name+/*" "+std::to_string(order_index)+*/ " ("+class_name+") "+imported_or_not_str);

    std::string unique_id_actor = editing_name ?
        std::string("###Actor"+std::to_string(editor_id)).c_str() :
        (visible_text+"###Actor"+std::to_string(editor_id)).c_str();

    bool tree_node_opened = false;
    if(import_mode == ImportModes::UNWRAPPED){
        std::string label = std::string("###ActorTree"+std::to_string(editor_id));

        //if(level->actors_with_stable_id.at(_level_editor_tab->actor_dedicated_to_viewport)->parent->editor_id == this->editor_id) ImGui::SetNextItemOpen(true);
        tree_node_opened = ImGui::TreeNodeEx(label.c_str(), ImGuiTreeNodeFlags_SpanTextWidth);

        ImGui::SameLine();
    }

    if(editing_name){
        ImGui::SameLine();
        ImGui::InputText(("###EditText"+std::to_string(editor_id)).c_str(),&editor_name);

        if((ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()) || ImGui::IsKeyPressed(ImGuiKey_Enter)){
            editing_name = false;

            //Todo: Store UndoRedo action for name changes
        }
    }
    else{
        bool selectable_text = ImGui::Selectable(unique_id_actor.c_str(),&is_selected, ImGuiSelectableFlags_None, ImVec2(ImGui::CalcTextSize(visible_text.c_str()).x,ImGui::GetFontSize()));

        if(selectable_text){
            _level_editor_tab->actor_dedicated_to_viewport = this->editor_id;
            _level_editor_tab->selected_actors.clear();

            if(!ImGui::IsKeyDown(ImGuiKey_LeftShift)){
                if(_editor->active_tab){
                    UFOEngineStudio::LevelEditorTab* level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(_editor->active_tab);
                    level_editor_tab->reset_selection_status = true;
                    should_be_selected = is_selected;

                }

            }

        }

    }

    if(ImGui::BeginPopupContextItem(("Options###Options"+std::to_string(editor_id)).c_str())){

        if(ImGui::MenuItem("Edit")){
            ufo::Actor* new_actor_place_holder = AddActor<UFOEngineStudio::NewActorPlaceHolder>(Vector2f(0.0f, 0.0f));

            _level_editor_tab->actor_dedicated_to_viewport = new_actor_place_holder->editor_id;
            level->actors_with_stable_id.emplace(new_actor_place_holder->editor_id, new_actor_place_holder);

        }
        if(ImGui::MenuItem("Rename")){
            TurnOnEditMode();
        }
        if(!is_top_actor_in_editor && !unremovable){
            if(ImGui::MenuItem("Delete")){
                level->RemoveFutureChanges();

                level->level_changes.push_back(std::make_unique<ufo::ActorChange_RemoveActor>(_level_editor_tab, this->editor_id, this->parent->editor_id));

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

    if(adding_new_actor){

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

                    level->level_changes.push_back(std::make_unique<ufo::ActorChange_AddActor>(_level_editor_tab,inst.get()->editor_id, this->editor_id));

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

        const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("ActorDragDrop");
        if(payload_data && !is_selected){
            if(_editor->active_tab){
                UFOEngineStudio::LevelEditorTab* level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(_editor->active_tab);

                std::sort(level_editor_tab->drag_dropped_actors.begin(), level_editor_tab->drag_dropped_actors.end(), [](DraggedActorWhereAbouts& _first, DraggedActorWhereAbouts& _second){
                   return _second.index > _first.index;
                });

                std::unique_ptr multiple_actor_change = std::make_unique<ufo::ActorChange_MultipleActorChange>();

                level->RemoveFutureChanges();

                for(int where_abouts_index = level_editor_tab->drag_dropped_actors.size()-1; where_abouts_index != -1; where_abouts_index--){

                    const auto dragged_actor_where_abouts_ = level_editor_tab->drag_dropped_actors[where_abouts_index];

                    Actor* drag_dropped_actor = dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index].get();

                    multiple_actor_change->changes.push_back(std::make_unique<ufo::ActorChange_Move>(
                        _level_editor_tab,
                        drag_dropped_actor->editor_id,
                        drag_dropped_actor->parent->editor_id,
                        drag_dropped_actor->order_index,
                        this->editor_id,
                        dragged_actor_where_abouts_.index));

                    drag_dropped_actor->parent = this;
                }

                level->level_changes.push_back(std::move(multiple_actor_change));

                for(const auto& dragged_actor_where_abouts_ : level_editor_tab->drag_dropped_actors){

                    new_actor_queue.push_back(std::move(dragged_actor_where_abouts_.parent->actors[dragged_actor_where_abouts_.index]));

                }

                for(int where_abouts_index = level_editor_tab->drag_dropped_actors.size()-1; where_abouts_index != -1; where_abouts_index--){

                    const auto dragged_actor_where_abouts_ = level_editor_tab->drag_dropped_actors[where_abouts_index];

                    dragged_actor_where_abouts_.parent->actors.erase(dragged_actor_where_abouts_.parent->actors.begin()+dragged_actor_where_abouts_.index);

                }

                level_editor_tab->drag_dropped_actors.clear();
            }

        }

        ImGui::EndDragDropTarget();
    }


    OnAdditionalButtonsForTreeItem();

    if(tree_node_opened){

        OnUpdateEditorTree(_index);

        for(int i = 0; i < (int)actors.size(); i++){

            actors[i]->UpdateEditorTree(_editor,_level_editor_tab,i);

        }


        ImGui::TreePop();
    }
}

void Actor::GetSelectedActors(std::vector<int>& _selected_actors, ufo::Rectangle _selection_rectangle_world_space){
    if(!ImGui::IsKeyDown(ImGuiKey_LeftShift)) is_selected_in_viewport = false;

    if(
        ufo::Maths::RectangleVsPoint(_selection_rectangle_world_space , GetGlobalPosition())
        && editor_name != "ControllableCamera (Editor Tool)" && editor_name != "SpawnCursor (Editor Tool)"
    ){
        _selected_actors.push_back(this->editor_id);
        return;
    }

    if(import_mode == ImportModes::WRAPPED) return;

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

bool Actor::InputFloatWithUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,const std::string& _id, std::string _name){
    float value_before_edit = *float_handles.at(_name);
    if(ImGui::InputFloat(_id.c_str(), float_handles.at(_name))){

        level->RemoveFutureChanges();

        level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableFloatHandle>(
            _level_editor_tab, editor_id, _name,value_before_edit, *float_handles.at(_name)));

        return true;
    }

    return false;
}

//This function is currently untested
bool Actor::InputIntWithUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,const std::string& _id, std::string _name){
    int value_before_edit = *int_handles.at(_name);
    if(ImGui::InputInt(_id.c_str(), int_handles.at(_name))){

        level->RemoveFutureChanges();

        level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableIntHandle>(_level_editor_tab, editor_id, _name, value_before_edit, *int_handles.at(_name)));

        return true;
    }

    return false;
}

//This function is currently untested
bool Actor::InputTextWithUndoAndRedo(UFOEngineStudio::LevelEditorTab* _level_editor_tab ,const std::string& _id, std::string _name){
    std::string value_before_edit = *string_handles.at(_name);
    if(ImGui::InputText(_id.c_str(), string_handles.at(_name))){

        level->RemoveFutureChanges();

        level->level_changes.push_back(std::make_unique<ufo::ActorChange_CustomVariableStringHandle>(_level_editor_tab, editor_id, _name, value_before_edit, *string_handles.at(_name)));

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
        Actor* actor = actor = GetActor(find_actor_search_field);

        if(actor) ImGui::Text("Found actor");
    }

    InputFloatWithUndoAndRedo(_level_editor_tab,
        std::string("local_position.x###local_position.x"+editor_name+std::to_string(_index)).c_str(), "local_position.x");
    InputFloatWithUndoAndRedo(_level_editor_tab,
        std::string("local_position.y###local_position.y"+editor_name+std::to_string(_index)).c_str(), "local_position.y");

    for(int i = 0; i < editor_properties.size(); i++){
        editor_properties[i]->Update(_level_editor_tab ,this, editor_name, i);
    }

}

void Actor::ViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    OnViewProperties(_level_editor_tab,_index);
    _level_editor_tab->currently_viewed_properties_actor_name = editor_name;

}

void Actor::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Actor::OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Actor::OnUtiliseAssetManager([[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

Actor* Actor::GetFocusedActor([[maybe_unused]] Vector2f _mouse_position_over_screenspace){
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

void Actor::UpdateEditorViewport([[maybe_unused]] UFOEngineStudio::Editor* _editor, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

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

void Actor::DrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    OnDrawGizmos(_graphics, _camera, _level_editor_tab);
    for(const auto& actor : actors){
        actor->DrawGizmos(_graphics, _camera, _level_editor_tab);
    }
}

void Actor::OnDrawGizmos([[maybe_unused]] ufo::Graphics* _graphics, [[maybe_unused]] Camera* _camera, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

#endif //UFO-Engine Studio

}
