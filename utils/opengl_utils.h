#pragma once

#include <string>
#include "../glad/include/glad/glad.h"
#include "../utils/console.h"

inline void GetGLError(const char *_file, int _line){

    GLenum error_code;

    while((error_code = glGetError()) != GL_NO_ERROR){

        std::string error;

        switch (error_code)
        {
        case GL_INVALID_ENUM: error = "INVALID_ENUM"; break;
        case GL_INVALID_VALUE: error = "INVALID_VALUE"; break;
        case GL_INVALID_OPERATION: error = "INVALID_OPERATION"; break;
        case GL_STACK_OVERFLOW: error = "STACK_OVERFLOW"; break;
        case GL_STACK_UNDERFLOW: error = "STACK_UNDERFLOW"; break;
        case GL_OUT_OF_MEMORY:  error = "OUT_OF_MEMORY"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
        }

        Console::Print(error + " " + std::string(_file) + " " + std::to_string(_line) + "\n");
    }
    
}