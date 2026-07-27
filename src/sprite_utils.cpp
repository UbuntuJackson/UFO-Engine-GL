#include <string>
#include "openglv4_5_asset_manager.h"
#include "texture_2d.h"
#include "../ufo_maths/ufo_maths.h"
#include "sprite_utils.h"

namespace ufo::SpriteUtils{

ufo::Rectangle
GetRectangleFromPositionAndFrameSize(int _x, int _y, Vector2f _frame_size){
    ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
    return rect;
}

ufo::Rectangle
GetFrameFromSpriteSheet(ufo::OpenGLv4_5_AssetManager* _asset_manager, std::string _sprite_key, int _frame, Vector2f _frame_size){

    ufo::Texture2D& texture = _asset_manager->textures.at(_sprite_key);

    int fx = 1;
    int fy = 1;
    //To be fixed, clamp the values for height and width
    if(_frame_size.x > 0.0f && _frame_size.y > 0.0f &&
        _frame_size.x <= texture.width && _frame_size.y <= texture.height){
        fx = (int)_frame % (texture.width/(int)_frame_size.x); //1 can only give me x = 0
        fy = (int)_frame / (texture.width/(int)_frame_size.x);
    }
    return GetRectangleFromPositionAndFrameSize(
        fx,
        fy,
        _frame_size); //1 can only give y = 1
}

}
