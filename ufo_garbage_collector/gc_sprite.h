#include "gc_actor.h"
#include "object.h"
#include "dynamic_array.h"
#include "../ufo_maths/ufo_maths.h"
#include "../src/graphics.h"
#include "../src/camera.h"

namespace ufo::gc{

class Sprite : public Actor{
public:
    Vector2f local_position;
    std::string key;
    Vector2f offset;
    Vector2f frame_size;
    Vector2f scale = {1.0f, 1.0f};
    float rotation = 0.0f;
    float current_frame_index = 0.0f;
    float number_of_frames = 1.0f;
    bool visible = true;
    ufo::Colour tint = olc::WHITE;

    Sprite(Vector2f _local_position): Actor(_local_position){}

    void OnUpdate(float _delta_time){

    }

    ufo::Rectangle
    GetRectangle(int _x, int _y, Vector2f _frame_size){
        ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
        return rect;
    }

    ufo::Rectangle
    GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size){
        int fx = 1;
        int fy = 1;
        //To be fixed, clamp the values for height and width
        if(_frame_size.x > 0.0f && _frame_size.y > 0.0f &&
            _frame_size.x <= engine->asset_manager.textures.at(_sprite_key).width && frame_size.y <= engine->asset_manager.textures.at(_sprite_key).height){
            fx = (int)_frame % (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x); //1 can only give me x = 0
            fy = (int)_frame / (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x);
        }
        return GetRectangle(
            fx,
            fy,
            _frame_size); //1 can only give y = 1
    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera){
        if(!visible) return;

        ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(key,current_frame_index,frame_size);
        _graphics->DrawPartialSprite(
            key,
            _camera->Transform(GetGlobalPosition()),
            /*size,*/
            offset,
            scale*_camera->scale,
            sample_rectangle.position,
            sample_rectangle.size,
            rotation,
            tint
        );
    }

};

}
