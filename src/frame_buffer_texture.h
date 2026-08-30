#pragma once

#include "texture_2d.h"
#include "../utils/console.h"
#include "ufo_macros.h"

namespace ufo{

class FrameBufferTexture : public Texture2D{
public:
    bool is_initialised = false;

    unsigned int CreateFrameBuffer(int _width, int _height, bool& _failed_to_create_frame_buffer_object);

    void BindFrameBuffer(unsigned int _frame_buffer_object);
    void UnbindFrameBuffer(unsigned int _frame_buffer_object);

    ~FrameBufferTexture();

};

}
