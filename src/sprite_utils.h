#pragma once
#include <string>
#include "openglv4_5_asset_manager.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo::SpriteUtils{

ufo::Rectangle
GetRectangleFromPositionAndFrameSize(int _x, int _y, Vector2f _frame_size);

ufo::Rectangle
GetFrameFromSpriteSheet(ufo::OpenGLv4_5_AssetManager* _asset_manager, std::string _sprite_key, int _frame, Vector2f _frame_size);

}
