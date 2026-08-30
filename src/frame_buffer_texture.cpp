#include "frame_buffer_texture.h"
#include "texture_2d.h"

namespace ufo{

unsigned int FrameBufferTexture::CreateFrameBuffer(int _width, int _height, bool& _failed_to_create_frame_buffer_object){
    unsigned int frame_buffer_object;

    width = _width;
    height = _height;

    glGenFramebuffers(1, &frame_buffer_object);
    glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_object);

    if(is_initialised) glDeleteTextures(1, &id);

    glGenTextures(1, &id);

    is_initialised = true;

    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height,
        0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, id,0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Error, framebuffer no good");
        _failed_to_create_frame_buffer_object = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Framebuffer created.");

    return frame_buffer_object;
}

void FrameBufferTexture::BindFrameBuffer(unsigned int _frame_buffer_object){
    glBindFramebuffer(GL_FRAMEBUFFER, _frame_buffer_object);
    //Console::PrintLine(FBO, RBO);
}

void FrameBufferTexture::UnbindFrameBuffer(unsigned int _frame_buffer_object){
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBufferTexture::~FrameBufferTexture(){
    //if(is_initialised) glDeleteTextures(1, &id);
}

}
