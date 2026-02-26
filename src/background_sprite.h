#pragma once

#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "sprite.h"
#include "graphics.h"
#include "camera.h"
#include "engine.h"

namespace ufo{

class BackgroundSprite : public Sprite{
public:
    Vector2f parallax = Vector2f(0.5f,0.5f);

    BackgroundSprite(Vector2f _local_position) : Sprite("placeholder_icon", _local_position, Vector2f(0.0f, 0.0f), Vector2f(16.0f, 16.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f){
        class_name = "ufo::BackgroundSprite";
        base_class_name = class_name;
    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera) override {
        if(!visible) return;

        int number_of_background_sprites = engine->width / (int)frame_size.x + 1;

        for(int i = 0; i < number_of_background_sprites+1; i++){

            Vector2f screen_position = Vector2f(_camera->GetGlobalPosition().x/parallax.x+i*frame_size.x, 0.0f);

            screen_position.x = ufo::Maths::Wrap(screen_position.x, -frame_size.x*1.0f, 1.0f*frame_size.x*number_of_background_sprites);

            ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(key,current_frame_index,frame_size);
            _graphics->DrawPartialSprite(
                key,
                screen_position,
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
