#pragma once

#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "sprite.h"
#include "graphics.h"
#include "camera.h"

namespace ufo{

class BackgroundSprite : public Sprite{
public:
    Vector2f parallax = Vector2f(1.5f,1.5f);

    BackgroundSprite(Vector2f _local_position) : Sprite("placeholder_icon", _local_position, Vector2f(0.0f, 0.0f), Vector2f(16.0f, 16.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f){

    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera) override {
        if(!visible) return;

        for(int i = 0; i < 3; i++){

            Vector2f screen_position = _camera->GetGlobalPosition() +frame_size*(float)i*parallax;

            ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(key,current_frame_index,frame_size);
            _graphics->DrawPartialSprite(
                key,
                Vector2f(ufoMaths::Wrap(screen_position.x, -_camera->viewport.size.x, _camera->viewport.size.x),0.0f)),
                /*size,*/
                offset,
                scale*_camera->scale,
                sample_rectangle.position,
                sample_rectangle.size,
                rotation,
                tint
            );
        }
    }

};

}
