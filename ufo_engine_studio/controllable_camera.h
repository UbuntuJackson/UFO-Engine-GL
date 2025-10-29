#pragma once
#include <camera.h>
#include <control_settings.h>
#include <level.h>
#include <engine.h>

class ControllableCamera : public Camera{
public:

    ufo::Controls* controls = nullptr;

    ControllableCamera(Vector2f _local_position) : Camera(_local_position){}

    void OnSpawn(){
        Camera::OnSpawn();

        controls = level->GetControls();
    }

    void OnUpdate(float _delta_time){
        Camera::OnUpdate(_delta_time);

        if(engine->keyboard.GetKey(controls->GetControl("right")).is_held) local_position.x += _delta_time * 100.0f;
        if(engine->keyboard.GetKey(controls->GetControl("left")).is_held) local_position.x -= _delta_time * 100.0f;
        if(engine->keyboard.GetKey(controls->GetControl("up")).is_held) local_position.y -= _delta_time * 100.0f;
        if(engine->keyboard.GetKey(controls->GetControl("down")).is_held) local_position.y += _delta_time * 100.0f;
    }

};