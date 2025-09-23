#include <vector>
#include <memory>
#include "actor.h"
#include "level.h"
#include "graphics.h"
#include "colour_rectangle.h"
#include "../random/random_number_generator.h"
#include "colour_circle.h"
#include "camera.h"
#include "sprite_renderer.h"
#include "../shapes/rectangle.h"

Level::Level() : Actor(Vector2f(0.0f, 0.0f)){}

ufo::Controls* Level::GetControls(){
    if(control_handout_counter == engine->control_settings.controls.size()-1) return nullptr;

    control_handout_counter++;
    return &engine->control_settings.controls[control_handout_counter];
}

void
Level::Load(){
    for(int i = 0; i < 200; i++){
        
        auto npc = AddActor<ColourCircle>(Vector2f(
            RandomNumberGenerator::Get().RandomFloat(0.0f, size.x),
            RandomNumberGenerator::Get().RandomFloat(0.0f, size.y)));
        npc->colour = ufo::Colour(
            RandomNumberGenerator::Get().RandomInt(0, 205),
            RandomNumberGenerator::Get().RandomInt(0, 100),
            RandomNumberGenerator::Get().RandomInt(0, 205)
        );

        npc->radius = RandomNumberGenerator::Get().RandomFloat(0.0f, 20.0f);
    }
    
    /*auto actor2 = AddActor<Player>(Vector2f(
            100.0f, 50.0f));
    
    //actor2->is_dead = true;
    
    actor2->colour = ufo::Colour(255, 255, 0, 255);*/

}

void Level::UpdatePhrase(float _delta_time){
    AddNewActors();

    for(const auto& actor : actors){
        actor->Update(_delta_time);
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
}