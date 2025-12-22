#include <map>
#include <memory>
#include <string>
#include "level.h"
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../utils/console.h"
#include "sprite.h"
#include "../imgui/imgui.h"
#include "../utils/console.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include <garbage_collector.h>
#include <gc_json.h>
#include <engine_memory.h>
#include "../ufo_engine_studio/editor.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include <sstream>

Actor::Actor(Vector2f _local_position) : local_position{_local_position}{
    editor_id = editor_id_counter++;
    editor_name = "@Instance"+class_name+std::to_string(editor_id);

    InitEditorProperties();
}

Vector2f Actor::GetGlobalPosition(){
    if(parent == nullptr){
        return local_position;
    }
    return local_position + parent->GetGlobalPosition();
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

    for(const auto& actor : actors){
        actor->Update(_delta_time);
    }

    OnUpdate(_delta_time);

    if(should_be_sorted){
        SortActors();
        Console::PrintLine("Sorting Actors");
    }
}

void Actor::WidgetDraw(ufo::Graphics* _graphics){
    OnWidgetDraw(_graphics);
    for(const auto& actor : actors){
        actor->WidgetDraw(_graphics);
    }
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

void Actor::InsertActors(){
    for(auto&& inserted_actor : inserted_actor_queue){
        actors.insert(actors.begin()+inserted_actor.index, std::move(inserted_actor.actor));
    }

    inserted_actor_queue.clear();

    for(const auto& actor : actors){
        actor->InsertActors();
    }
}

void Actor::SetOrderIndex(int _index){
    if(parent) parent->should_be_sorted = true;
    order_index = _index;
}

void Actor::SortActors(){
    std::sort(actors.begin(), actors.end(), [this](const auto& _a, const auto& _b){
        return _a->order_index < _b->order_index;
    });

    for(int i = 0; i < actors.size(); i++){
        actors[i]->order_index = i;
    }

    should_be_sorted = false;
}

void Actor::TurnOnEditMode(){
    editing_name = true;
    old_editor_name = editor_name;
}

void Actor::OnUpdateEditorTree(int _index){

}

void Actor::UpdateEditorTree(UFOEngineStudio::Editor* _editor,int _index){
    //if(ImGui::GetMousePos().y > _index * ImGui::GetStyle().ItemSpacing.y * )

    bool button_pressed = ImGui::InvisibleButton(std::string("###InvisibleButton"+editor_name+std::to_string(_index)).c_str(),ImVec2(100,3));

    if(ImGui::BeginDragDropTarget()){

        const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("ActorDragDrop");
        if(payload_data){
            DraggedActorWhereAbouts* dragged_actor_where_abouts_ = (DraggedActorWhereAbouts*)(payload_data->Data);

            dragged_actor_where_abouts_->parent->actors[dragged_actor_where_abouts_->index]->parent = parent;

            parent->inserted_actor_queue.push_back(InsertedActor{_index,std::move(dragged_actor_where_abouts_->parent->actors[dragged_actor_where_abouts_->index])});

            dragged_actor_where_abouts_->parent->actors.erase(dragged_actor_where_abouts_->parent->actors.begin()+dragged_actor_where_abouts_->index);

        }

        ImGui::EndDragDropTarget();
    }

    bool tree_node_opened = ImGui::TreeNodeEx(editing_name ? std::string("###Actor"+std::to_string(editor_id)).c_str() : std::string(editor_name+" ("+class_name+"("+base_class_name+")"+")"+"###Actor"+std::to_string(editor_id)).c_str());

    if(editing_name){
        ImGui::SameLine();
        ImGui::InputText(("###EditText"+std::to_string(editor_id)).c_str(),&editor_name);

        if((ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()) || ImGui::IsKeyPressed(ImGuiKey_Enter)){
            editing_name = false;
        }
    }
    else{
        if(ImGui::IsItemClicked(ImGuiMouseButton_Left)){
            _editor->set_all_actors_properties_open_to_false = true;
            should_open_properties = true;
        }

    }

    if(ImGui::BeginPopupContextItem(("Options###Options"+std::to_string(editor_id)).c_str())){
        if(ImGui::MenuItem("Rename")){
            TurnOnEditMode();
        }
        if(ImGui::MenuItem("Delete")){
            is_dead = true;

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
        for(const auto& [k,v] : _editor->spawnable_actor_map){
            if(ImGui::Button(std::string("Add "+k).c_str())){
                auto inst = v->Spawn(_editor);
                inst->class_name = k;
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

        ImGui::EndDragDropSource();
    }

    if(ImGui::BeginDragDropTarget()){

        const ImGuiPayload* payload_data = ImGui::AcceptDragDropPayload("ActorDragDrop");
        if(payload_data){
            DraggedActorWhereAbouts* dragged_actor_where_abouts_ = (DraggedActorWhereAbouts*)(payload_data->Data);

            dragged_actor_where_abouts_->parent->actors[dragged_actor_where_abouts_->index]->parent = this;

            new_actor_queue.push_back(std::move(dragged_actor_where_abouts_->parent->actors[dragged_actor_where_abouts_->index]));

            dragged_actor_where_abouts_->parent->actors.erase(dragged_actor_where_abouts_->parent->actors.begin()+dragged_actor_where_abouts_->index);

        }

        ImGui::EndDragDropTarget();
    }

    /*if(ImGui::IsItemHovered()){
        Console::PrintLine("ImGui::GetItemRectMin.y", ImGui::GetItemRectMin().y);
    }*/

    if(tree_node_opened){

        OnUpdateEditorTree(_index);

        for(int i = 0; i < actors.size(); i++){

            actors[i]->UpdateEditorTree(_editor,i);

        }

        ImGui::TreePop();
    }
}

void Actor::InitEditorProperties(){
    editor_properties.push_back(std::make_unique<EditorPropertyFloatHandle>("x","x",&(local_position.x)));
    editor_properties.push_back(std::make_unique<EditorPropertyFloatHandle>("y","y",&(local_position.y)));
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

void Actor::RemoveAndAddEditorPropertiesDuringRuntime(UFOEngineStudio::Editor* _editor){
    if(_editor->spawnable_actor_map.count(class_name)){
        UFOEngineStudio::Editor::AdvancedActorSpawner* advanced_spawner_of_this_class = _editor->spawnable_actor_map.at(class_name).get();

        std::map<std::string, std::unique_ptr<UFOEngineStudio::Editor::EditorProperty>> properties_template;

        std::map<std::string, UFOEngineStudio::Editor::EditorProperty*> properties_of_this;

        for(const auto& property : advanced_spawner_of_this_class->properties){
             properties_template.emplace(property->variable_name,property->Copy());
        }

        for(const auto& property : editor_properties){
            properties_of_this.emplace(property->variable_name, property.get());
        }

        for(const auto& [k,v] : properties_template){
            if(!properties_of_this.count(k)){
                editor_properties.push_back(v->Copy());
            }
        }

        for(const auto& [k,v] : properties_of_this){
            if(!properties_template.count(k) && k != "x" && k != "y"){
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

void Actor::OnViewProperties(int _index){

    bool search_field_active = true;

    if(search_field_active){
        ImGui::InputText("FindActor...", &find_actor_search_field, ImGuiInputTextFlags_EnterReturnsTrue);
        Actor* actor = GetActor(find_actor_search_field);
        if(actor != nullptr){

            if(ImGui::Button(std::string("Found actor: "+find_actor_search_field).c_str())){

            }
        }
    }

    for(int i = 0; i < editor_properties.size(); i++){
        editor_properties[i]->Update(editor_name, i);
    }
    //ImGui::Text("local_position");
    //ImGui::InputFloat(std::string("x###Propertyx"+std::to_string(editor_id)).c_str(), &local_position.x);
    //ImGui::InputFloat(std::string("y###Propertyy"+std::to_string(editor_id)).c_str(), &local_position.y);
}

void Actor::ViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    if(properties_open){
        OnViewProperties(_index);
        _level_editor_tab->currently_viewed_properties_actor_name = editor_name;
    }
    for(int i = 0; i < actors.size(); i++){
        actors[i]->ViewProperties(_level_editor_tab,i);
    }
}

void Actor::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor){

}

void Actor::UpdateEditorViewport(UFOEngineStudio::Editor* _editor){
    for(const auto& actor : actors){
        actor->UpdateEditorViewport(_editor);
    }

    OnUpdateEditorViewport(_editor);
}

ufo::gc::JsonMap* Actor::GetAsJson(ufo::GarbageCollector* _gc){
    ufo::gc::JsonMap* this_actor = _gc->New<ufo::gc::JsonMap>();
    this_actor->map.emplace("name", _gc->New<ufo::gc::JsonString>(editor_name));
    this_actor->map.emplace("base_class_name", _gc->New<ufo::gc::JsonString>(base_class_name));
    this_actor->map.emplace("class_name", _gc->New<ufo::gc::JsonString>(class_name));
    auto j_custom_editor_properties = _gc->New<ufo::gc::JsonMap>();
    this_actor->map.emplace("custom_editor_properties", j_custom_editor_properties);

    for(const auto& property : editor_properties){
        if(property->variable_name == "x" || property->variable_name == "y"){
            this_actor->map.emplace(property->variable_name, property->GetJson(_gc));
        }
        else j_custom_editor_properties->map.emplace(property->variable_name,property->GetJson(_gc));
    }

    ufo::gc::JsonArray* children = _gc->New<ufo::gc::JsonArray>();

    for(const auto& actor : actors){
        children->array.push_back(actor->GetAsJson(_gc));
    }

    this_actor->map.emplace("actors", children);

    return this_actor;
}
