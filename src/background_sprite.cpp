#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "sprite.h"
#include "graphics.h"
#include "camera.h"
#include "engine.h"
#include "background_sprite.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#endif

namespace ufo{

BackgroundSprite::BackgroundSprite(Vector2f _local_position) : Sprite(_local_position,"placeholder_icon", Vector2f(0.0f, 0.0f), Vector2f(16.0f, 16.0f), Vector2f(1.0f, 1.0f), 0.0f, 0.0f){
    class_name = "ufo::BackgroundSprite";
    base_class_name = class_name;
#ifdef UFO_ENGINE_STUDIO
    float_handles.emplace("parallax.x",&parallax.x);
    float_handles.emplace("parallax.y",&parallax.y);
#endif

}

void BackgroundSprite::OnDraw(ufo::Graphics* _graphics, Camera* _camera){
    if(!visible) return;

    int scaled_frame_size_x = frame_size.x * _camera->scale;

    int number_of_background_sprites = engine->width / scaled_frame_size_x + 1;

    for(int i = 0; i < number_of_background_sprites+1; i++){

        Vector2f screen_position = Vector2f(-_camera->GetGlobalPosition().x*parallax.x+i*scaled_frame_size_x, 0.0f);

        screen_position.x = ufo::Maths::Wrap(screen_position.x, -scaled_frame_size_x*1.0f, 1.0f*scaled_frame_size_x*number_of_background_sprites);

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
            tint, shader_key
        );
    }
}

ufo::gc::JsonMap* BackgroundSprite::GetAsJson(ufo::GarbageCollector* _gc){
    ufo::gc::JsonMap* parent_class_as_json = Sprite::GetAsJson(_gc);

    parent_class_as_json->map.emplace("parallax_x", _gc->New<ufo::gc::JsonNumber>(parallax.x));
    parent_class_as_json->map.emplace("parallax_y", _gc->New<ufo::gc::JsonNumber>(parallax.y));

    return parent_class_as_json;
}

void BackgroundSprite::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){

    Sprite::OnLoadDefaultProperties(_json);

    _json->TryToGetValueAsFloat("parallax_x", parallax.x);
    _json->TryToGetValueAsFloat("parallax_y", parallax.y);

}

#ifdef UFO_ENGINE_STUDIO
void BackgroundSprite::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    Sprite::OnViewProperties(_level_editor_tab, _index);

    InputFloatWithUndoAndRedo(_level_editor_tab,"parallax.x", "parallax.x");
    InputFloatWithUndoAndRedo(_level_editor_tab,"parallax.y", "parallax.y");

}
#endif

}
