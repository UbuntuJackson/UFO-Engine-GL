#include <map>
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

Actor::Actor(Vector2f _local_position) : local_position{_local_position}{
    editor_id = editor_id_counter++;
    editor_name = "Actor"+std::to_string(editor_id);

    InitEditorProperties();
}

Vector2f Actor::GetGlobalPosition(){
    if(parent == nullptr){
        return local_position;
    }
    return local_position + parent->GetGlobalPosition();
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

void Actor::UpdateEditorTree(int _index){
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

    bool tree_node_opened = ImGui::TreeNodeEx(editing_name ? std::string("###Actor"+std::to_string(editor_id)).c_str() : std::string(editor_name+" ("+class_name+")"+"###Actor"+std::to_string(editor_id)).c_str());
    
    if(editing_name){
        ImGui::SameLine();
        ImGui::InputText(("###EditText"+std::to_string(editor_id)).c_str(),&editor_name);

        if((ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsItemHovered()) || ImGui::IsKeyPressed(ImGuiKey_Enter)){
            editing_name = false;
        }
    }
    else{
        if(ImGui::IsItemClicked(ImGuiMouseButton_Left)){
            properties_open = true;
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
        ImGui::EndPopup();
    }

    if(adding_new_actor){
        //Read from json somehow to add the attributes, however tf that is gonna happen

        ImGui::Begin("Adding Actor");
        ImGui::Text("Actor: Actor is the baseclass for all objects");
        if(ImGui::Button("Add###Add0")){
            AddActor<Actor>(Vector2f(0.0f, 0.0f));
            adding_new_actor = false;
        }
        ImGui::Text("Sprite: An ordinary sprite for static images");
        if(ImGui::Button("Add###Add1")){
            AddActor<Sprite>("placeholder_icon",Vector2f(0.0f, 0.0f), Vector2f(0.0f, 0.0f), Vector2f(32.0f, 32.0f), Vector2f(1.0f, 1.0f), 0.0f, 0);
            adding_new_actor = false;
        }
        ImGui::End();
    }

    if(ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)){
        dragged_actor_where_abouts = DraggedActorWhereAbouts{parent, _index};

        ImGui::SetDragDropPayload("ActorDragDrop", &dragged_actor_where_abouts, sizeof(DraggedActorWhereAbouts));
        ImGui::Text(editor_name.c_str());

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

            actors[i]->UpdateEditorTree(i);
            
        }
        
        ImGui::TreePop();
    }
}

void Actor::OnViewProperties(int _index){
    for(int i = 0; i < editor_properties.size(); i++){
        editor_properties[i]->Update(editor_name, i);
    }
    //ImGui::Text("local_position");
    //ImGui::InputFloat(std::string("x###Propertyx"+std::to_string(editor_id)).c_str(), &local_position.x);
    //ImGui::InputFloat(std::string("y###Propertyy"+std::to_string(editor_id)).c_str(), &local_position.y);
}

ufo::gc::JsonMap* Actor::GetAsJson(ufo::GarbageCollector* _gc){
    ufo::gc::JsonMap* this_actor = _gc->New<ufo::gc::JsonMap>();
    this_actor->map.emplace("name", _gc->New<ufo::gc::JsonString>(editor_name));
    this_actor->map.emplace("type", _gc->New<ufo::gc::JsonString>(class_name));
    
    for(const auto& property : editor_properties){
        this_actor->map.emplace(property->variable_name, property->GetJson(_gc));
    }

    ufo::gc::JsonArray* children = _gc->New<ufo::gc::JsonArray>();

    for(const auto& actor : actors){
        children->array.push_back(actor->GetAsJson(_gc));
    }

    this_actor->map.emplace("actors", children);

    return this_actor;
}