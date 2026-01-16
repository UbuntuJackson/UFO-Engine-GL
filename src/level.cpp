#include <vector>
#include <memory>
#include "actor.h"
#include "level.h"
#include "engine.h"
#include "graphics.h"
#include "camera.h"
#include "../glad/include/glad/glad.h"
#include "openglv4_5_graphics.h"
#include "../shapes/rectangle.h"
#include "../tilemap/tileset_manager.h"
#include "text.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"

Level::Level() : Actor(Vector2f(0.0f, 0.0f)){
    class_name = "Level";
    base_class_name = class_name;
}

void Level::OnSpawn(){
    /*auto ut = std::make_unique<ufo::Text>();
    ut->SetText("UFO-Engine 風船");
    widget->AddWidgetUniquePtr(std::move(ut));

    auto ut2 = std::make_unique<ufo::Text>();
    ut2->local_position = Vector2f(200.0f, 300.0f);
    ut2->SetText("すばらしい");
    widget->AddWidgetUniquePtr(std::move(ut2));*/

    tileset_manager.engine = engine;

    tileset_manager.InitialiseTextures();
}

ufo::Controls* Level::GetControls(){
    if(control_handout_counter == engine->control_settings.controls.size()-1) return nullptr;

    control_handout_counter++;
    return &engine->control_settings.controls[control_handout_counter];
}

void
Level::Load(){

}

void Level::Update(float _delta_time){}

void Level::UpdatePhrase(float _delta_time){
    AddNewActors();

    InsertActors();

    //Run local OnUpdate function on level too
    OnUpdate(_delta_time);

    for(const auto& actor : actors){
        actor->Update(_delta_time);
    }

    if(should_be_sorted){
        SortActors();
        Console::PrintLine("Sorting Actors in Level");
    }

    for(const auto& camera : active_camera_handles){
        camera->HandleUpdate();
    }

    //Clean up handles before actually freeing the actor
    for(int i = active_camera_handles.size()-1; i != -1; i--){
        if(active_camera_handles[i]->is_dead){
            active_camera_handles.erase(active_camera_handles.begin()+i);
        }
    }

    CleanUpDeadActors();

    //widget->Update();
}

void Level::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    Vector2f min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition());
    Vector2f max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+size);

    ImGui::GetWindowDrawList()->AddRect(
        ImVec2(min.x, min.y),
        ImVec2(max.x, max.y), 0xFFFFFFFF, 1.0f,ImDrawFlags_RoundCornersAll);

    if(ImGui::IsItemClicked(0) && _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::PLACE){
        if(_editor->currently_selected_actor_type != ""){
            if(_editor->spawnable_actor_map.count(_editor->currently_selected_actor_type)){
                auto inst = _editor->spawnable_actor_map.at(_editor->currently_selected_actor_type)->Spawn(_editor);

                inst->local_position = active_camera_handles.back()->TransformScreenToWorld(_level_editor_tab->mouse_position_over_screenspace);

                AddActorUniquePtr(std::move(inst));
            }
        }
    }

}

bool Level::OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    return false;
}

void Level::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Level::DrawPhase(ufo::Graphics* _graphics){

    /*glViewport(0,0,engine->width/2,engine->height);
    _graphics->SetProjection(0.0f, engine->width/2, engine->height, 0.0f);
    for(const auto& actor : actors){
        actor->WidgetDraw(_graphics);
    }

    glViewport(engine->width/2,0,engine->width/2,engine->height);*/
    if(active_camera_handles.size() == 1){
        active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)engine->width,(float)engine->height));
        _graphics->SetProjection(0.0f, engine->width, engine->height, 0.0f);
    }

    if(active_camera_handles.size() == 2){

        /*if(ufoMaths::Distance2(active_camera_handles[0]->GetGlobalPosition(), active_camera_handles[1]->GetGlobalPosition()) < 400.0f){
            glViewport(0,0,engine->width,engine->height);
            _graphics->SetProjection(0.0f, engine->width,engine->height, 0.0f);

            for(const auto& actor : actors){
                actor->Draw(_graphics,active_camera_handles[1]);
            }

            return;
        }*/

        float delta_x = std::abs(active_camera_handles[0]->GetGlobalPosition().x - active_camera_handles[1]->GetGlobalPosition().x);
        float delta_y = std::abs(active_camera_handles[0]->GetGlobalPosition().y - active_camera_handles[1]->GetGlobalPosition().y);
        if(delta_x > 200 || delta_y > 150){
            if(delta_x >= delta_y){
                if(active_camera_handles[0]->GetGlobalPosition().x < active_camera_handles[1]->GetGlobalPosition().x){
                    active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)engine->width/2.0f,(float)engine->height));
                    active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f((float)engine->width/2.0f,0.0f),Vector2f((float)engine->width/2.0f,(float)engine->height));
                }

                if(active_camera_handles[0]->GetGlobalPosition().x >= active_camera_handles[1]->GetGlobalPosition().x){
                    active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)engine->width/2.0f,(float)engine->height));
                    active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f((float)engine->width/2.0f,0.0f),Vector2f((float)engine->width/2.0f,(float)engine->height));
                }
            }
            else{
                if(active_camera_handles[0]->GetGlobalPosition().y > active_camera_handles[1]->GetGlobalPosition().y){
                    active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f(engine->width,engine->height/2.0f));
                    active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f(0.0f,engine->height/2.0f),Vector2f(engine->width,engine->height/2.0f));
                }

                if(active_camera_handles[0]->GetGlobalPosition().y <= active_camera_handles[1]->GetGlobalPosition().y){
                    active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f(engine->width,engine->height/2.0f));
                    active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f,engine->height/2.0f),Vector2f(engine->width,engine->height/2.0f));
                }
            }
        }
        else{
            active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)engine->width,(float)engine->height));
            _graphics->SetProjection(0.0f, engine->width, engine->height, 0.0f);

            for(const auto& actor : actors){
                actor->Draw(_graphics,active_camera_handles[0]);
            }

            return;
        }
        //active_camera_handles[0]->viewport = ufo::Rectangle(Vector2f(0.0f ,0.0f),Vector2f((float)engine->width/2.0f,(float)engine->height));
        //active_camera_handles[1]->viewport = ufo::Rectangle(Vector2f((float)engine->width/2.0f,0.0f),Vector2f((float)engine->width/2.0f,(float)engine->height));
    }

    for(const auto& camera : active_camera_handles){
        glViewport(camera->viewport.position.x, camera->viewport.position.y, camera->viewport.size.x,camera->viewport.size.y);
        _graphics->SetProjection(0.0f, camera->viewport.size.x, camera->viewport.size.y, 0.0f);
        for(const auto& actor : actors){
            actor->Draw(_graphics,camera);
        }
    }

    glViewport(0,0,engine->width,engine->height);
    _graphics->SetProjection(0.0f, engine->width,engine->height, 0.0f);

    for(const auto& actor : actors){
        actor->WidgetDraw(_graphics);
    }

    /*_graphics->DrawPartialSprite(
        engine->sdl_text_texture,
        Vector2f(0.0f, 0.0f),
        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 0.0f),
        Vector2f(engine->sdl_text_texture.width, engine->sdl_text_texture.height),
        0.0f,
        ufo::Colour(255,255,255,255)
        );*/

    //widget->Draw(_graphics);

}

void Level::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    ImGui::InputFloat(std::string("size.x###size.x"+editor_name+std::to_string(_index)).c_str(), &size.x);
    ImGui::InputFloat(std::string("size.y###size.y"+editor_name+std::to_string(_index)).c_str(), &size.y);
}

void Level::DrawGizmosPhase(ufo::Graphics* _graphics, UFOEngineStudio::LevelEditorTab* _level_editor_tab){


    DrawGizmos(_graphics,active_camera_handles.back(), _level_editor_tab);

}
