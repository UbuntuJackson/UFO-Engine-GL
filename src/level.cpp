#include <vector>
#include <memory>
#include "actor.h"
#include "level.h"
#include "console.h"
#include "engine.h"
#include "gc_json.h"
#include "graphics.h"
#include "camera.h"
#include "../glad/include/glad/glad.h"
#include "openglv4_5_graphics.h"
#include "../shapes/rectangle.h"
#include "../tilemap/tileset_manager.h"
#include "text.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/im_vec.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"
#include "actor_undo_and_redo.h"
#endif

namespace ufo{

Level::Level() : Actor(Vector2f(0.0f, 0.0f)){
    class_name = "ufo::Level";
    base_class_name = class_name;

    level = this;
#ifdef UFO_ENGINE_STUDIO
    is_permanently_non_selectable = true;
#endif

}

void Level::OnSpawn(){

    tileset_manager.engine = engine;

    tileset_manager.InitialiseTextures();
}

ufo::Controls* Level::GetControls(){
    if(control_handout_counter == (int)engine->control_settings.controls.size()-1) return nullptr;

    control_handout_counter++;
    return &engine->control_settings.controls[control_handout_counter];
}

void
Level::Load(){

}

void Level::Update([[maybe_unused]] float _delta_time){}

void Level::UpdatePhase(float _delta_time){
    CleanUpDeadActors();

    AddNewActors();

    //Run local OnUpdate function on level too
    OnUpdate(_delta_time);

    if(!is_paused){
        for(const auto& actor : actors){
            actor->Update(_delta_time);
        }
    }
    else{
        for(const auto& actor : actors){
            actor->Pause(_delta_time);
        }
    }

    if(should_be_sorted){
        SortActors();
        Console::PrintLine("Sorting Actors in Level");
    }

    for(const auto& camera : active_camera_handles){
        camera->HandleUpdate();
    }

#ifdef UFO_ENGINE_STUDIO
    StashActors();
#endif

}

void Level::DrawPhase(ufo::Graphics* _graphics, int _width, int _height){
    engine->game_width = _width;
    engine->game_height = _height;

    if(engine->multi_player){
        if(active_camera_handles.size() == 1){
            active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)_width,(float)_height));
            _graphics->SetProjection(0.0f, _width, _height, 0.0f);
        }
        else if(active_camera_handles.size() == 2){

            float delta_x = std::abs(active_camera_handles[0]->GetGlobalPosition().x - active_camera_handles[1]->GetGlobalPosition().x);
            float delta_y = std::abs(active_camera_handles[0]->GetGlobalPosition().y - active_camera_handles[1]->GetGlobalPosition().y);
            if(delta_x > 200 || delta_y > 150){
                if(delta_x >= delta_y){
                    if(active_camera_handles[0]->GetGlobalPosition().x < active_camera_handles[1]->GetGlobalPosition().x){
                        active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)_width/2.0f,(float)_height));
                        active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f((float)_width/2.0f,0.0f),Vector2f((float)_width/2.0f,(float)_height));
                    }

                    if(active_camera_handles[0]->GetGlobalPosition().x >= active_camera_handles[1]->GetGlobalPosition().x){
                        active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)_width/2.0f,(float)_height));
                        active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f((float)_width/2.0f,0.0f),Vector2f((float)_width/2.0f,(float)_height));
                    }
                }
                else{
                    if(active_camera_handles[0]->GetGlobalPosition().y > active_camera_handles[1]->GetGlobalPosition().y){
                        active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f(_width,_height/2.0f));
                        active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f(0.0f,_height/2.0f),Vector2f(_width,_height/2.0f));
                    }

                    if(active_camera_handles[0]->GetGlobalPosition().y <= active_camera_handles[1]->GetGlobalPosition().y){
                        active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f(_width,_height/2.0f));
                        active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f,_height/2.0f),Vector2f(_width,_height/2.0f));
                    }
                }

                for(const auto& camera : active_camera_handles){
                    glViewport(camera->viewport.position.x, camera->viewport.position.y, camera->viewport.size.x,camera->viewport.size.y);
                    _graphics->SetProjection(0.0f, camera->viewport.size.x, camera->viewport.size.y, 0.0f);
                    for(const auto& actor : actors){
                        actor->Draw(_graphics,camera);
                    }
                }
            }
            else{
                active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)_width,(float)_height));
                _graphics->SetProjection(0.0f, _width, _height, 0.0f);

                for(const auto& actor : actors){
                    actor->Draw(_graphics,active_camera_handles[0]);
                }
            }

        }
    }
    else if(active_camera_handles.size() > 0){

        active_camera_handles.back()->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)_width,(float)_height));
        glViewport(0,0,_width,_height);
        _graphics->SetProjection(0.0f, _width,_height, 0.0f);

        for(const auto& actor : actors){
            actor->Draw(_graphics, active_camera_handles.back());
        }

    }

    glViewport(0,0,_width,_height);
    _graphics->SetProjection(0.0f, _width,_height, 0.0f);

}

void Level::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
    try{
        float size_x = _json->map.at("size_x")->AsFloat();
        float size_y = _json->map.at("size_y")->AsFloat();
        size.x = size_x;
        size.y = size_y;
    } catch(const std::exception& _error){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Could not find properties for json representing Level instance", _error.what());
    }

    ufo::gc::Json* j_tilesets = _json->map.at("tilesets");
    for(const auto& j_tileset : j_tilesets->AsArray()){

        tileset_manager.tileset_data.push_back(
            TilesetData{
                j_tileset->AsMap().at("name")->AsString(),
                (int)j_tileset->AsMap().at("columns")->AsFloat(),
                (int)j_tileset->AsMap().at("tileset_start_id")->AsFloat(),
                j_tileset->AsMap().at("image_width")->AsFloat(),
                j_tileset->AsMap().at("image_height")->AsFloat(),
                j_tileset->AsMap().at("tile_width")->AsFloat(),
                j_tileset->AsMap().at("tile_height")->AsFloat(),
                (int)j_tileset->AsMap().at("tile_count")->AsFloat()
            }
        );
    }
    if(_json->map.count("level_textures")){
        ufo::gc::Json* j_level_textures = _json->map.at("level_textures");
        for(ufo::gc::Json* j_texture : j_level_textures->AsArray()){

            if(!engine->asset_manager.textures.count(j_texture->AsString())){

                #ifdef UFO_ENGINE_STUDIO
                engine->asset_manager.LoadTexture(engine->editor.opened_directory_path + "/" + j_texture->AsString(), j_texture->AsString(), true);
                #else
                engine->asset_manager.LoadTexture(engine->game_directory + "/" + j_texture->AsString(), j_texture->AsString(), true);
                #endif

                if(!engine->asset_manager.textures.count(j_texture->AsString())) continue;

                engine->asset_manager.textures.at(j_texture->AsString()).is_global_asset = false;
                engine->asset_manager.textures.at(j_texture->AsString()).is_savable = true;
            }

            Console::PrintLine("j_level_texture",j_texture->AsString());

            level_textures.insert(j_texture->AsString());

        }

        for(std::string s : level_textures){
            Console::PrintLine("level_texture",s);
        }
    }

}

ufo::gc::JsonMap* Level::GetAsJson(ufo::GarbageCollector* _gc){

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
    ufo::gc::JsonArray* tilesets = _gc->New<ufo::gc::JsonArray>();

    for(const auto& tileset : tileset_manager.tileset_data){
        if(!tileset.is_loaded_from_path) continue;
        ufo::gc::JsonMap* j_tileset = _gc->New<ufo::gc::JsonMap>();
        j_tileset->map.emplace("name",_gc->New<ufo::gc::JsonString>(tileset.name));
        j_tileset->map.emplace("columns",_gc->New<ufo::gc::JsonNumber>(tileset.columns));
        j_tileset->map.emplace("tileset_start_id",_gc->New<ufo::gc::JsonNumber>(tileset.tileset_start_id));
        j_tileset->map.emplace("image_width",_gc->New<ufo::gc::JsonNumber>(tileset.image_width));
        j_tileset->map.emplace("image_height",_gc->New<ufo::gc::JsonNumber>(tileset.image_height));
        j_tileset->map.emplace("tile_width",_gc->New<ufo::gc::JsonNumber>(tileset.tile_width));
        j_tileset->map.emplace("tile_height",_gc->New<ufo::gc::JsonNumber>(tileset.tile_height));
        j_tileset->map.emplace("tile_count",_gc->New<ufo::gc::JsonNumber>(tileset.tile_count) );
        tilesets->array.push_back(j_tileset);
    }

    parent_class_as_json->map.emplace("tilesets",tilesets);

    ufo::gc::JsonArray* j_level_textures = _gc->New<ufo::gc::JsonArray>();
    for(const auto& texture : level->level_textures){

        j_level_textures->array.push_back(_gc->New<ufo::gc::JsonString>(texture));

    }

    parent_class_as_json->map.emplace("level_textures",j_level_textures);

    parent_class_as_json->map.emplace("size_x",_gc->New<ufo::gc::JsonNumber>(size.x));
    parent_class_as_json->map.emplace("size_y",_gc->New<ufo::gc::JsonNumber>(size.y));

    return parent_class_as_json;
}

#ifdef UFO_ENGINE_STUDIO

Actor* Level::OnGetFocusedActor([[maybe_unused]] Vector2f _mouse_position_over_screenspace) {
    return nullptr;
}

void Level::OnUpdateEditorViewport([[maybe_unused]] UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    const Vector2f pos_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition());
    const Vector2f pos_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+size);

    ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min), UFOEngineStudio::FromVector2fToImVec2(pos_max), 0xFF00FFFF);

}

bool Level::OnUpdateEditorViewportFocus([[maybe_unused]] UFOEngineStudio::Editor* _editor, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    return false;
}

void Level::Undo(){
    if(!(current_level_change < 0)){

        level_changes[current_level_change]->Undo();
        current_level_change--;
    }
}

void Level::Redo(){
    if(!(current_level_change >= (int)level_changes.size()-1)){

        current_level_change++;
        level_changes[current_level_change]->Redo();

    }
}

void Level::RemoveFutureChanges(){
    while((int)level_changes.size()-1 > current_level_change){
        Console::PrintLine("loop change stack",current_level_change, level_changes.size());
        level->level_changes.pop_back();
    }

    current_level_change++;
}

void Level::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    ImGui::InputFloat(std::string("size.x###size.x"+editor_name+std::to_string(_index)).c_str(), &size.x);
    ImGui::InputFloat(std::string("size.y###size.y"+editor_name+std::to_string(_index)).c_str(), &size.y);
}

void Level::OnDrawGizmos([[maybe_unused]] ufo::Graphics* _graphics,[[maybe_unused]]  Camera* _camera,[[maybe_unused]]  UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Level::DrawGizmosPhase([[maybe_unused]] ufo::Graphics* _graphics, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    if(_level_editor_tab->inspected_actor_id != ufo::Maths::NULL_ID){
        actors_with_stable_id.at(_level_editor_tab->inspected_actor_id)->OnDrawGizmos(engine->graphics.get(), active_camera_handles.back(), _level_editor_tab);
    }
    //DrawGizmos(_graphics,active_camera_handles.back(), _level_editor_tab);

}

#endif //UFO_ENGINE_STUDIO

}
