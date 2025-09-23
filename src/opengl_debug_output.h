#pragma once
#include "../utils/console.h"
#include "../glad/include/glad/glad.h"

void APIENTRY glDebugOutput(
    GLenum _source,
    GLenum _type,
    unsigned int _id,
    GLenum _severity,
    GLsizei _length,
    const char* _message,
    const void *_user_parameter)
{

    //Errors to ignore
    if(_id == 131169 || _id == 131185 || _id == 131218, _id == 131204) return;

    Console::PrintLine("Debug message (",_id,")",_message);

    switch(_source){
        case GL_DEBUG_SOURCE_API: Console::Print("Source: API"); break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: Console::Print("Source: Window System"); break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: Console::Print("Source: Shader Compiler"); break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: Console::Print("Source: Third Party"); break;
        case GL_DEBUG_SOURCE_APPLICATION: Console::Print("Source: Application"); break;
        case GL_DEBUG_SOURCE_OTHER: Console::Print("Source: Other"); break;
    }
    Console::PrintLine("");

    switch (_type)
    {
        case GL_DEBUG_TYPE_ERROR: Console::Print("Type: Error"); break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: Console::Print("Type: Deprecated Behaviour"); break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: Console::Print("Type: Undefined Behavour"); break;
        case GL_DEBUG_TYPE_PORTABILITY: Console::Print("Type: Portability"); break;
        case GL_DEBUG_TYPE_PERFORMANCE: Console::Print("Type: Performance"); break;
        case GL_DEBUG_TYPE_MARKER: Console::Print("Type: Marker"); break;
        case GL_DEBUG_TYPE_PUSH_GROUP: Console::Print("Type: Push Group"); break;
        case GL_DEBUG_TYPE_POP_GROUP: Console::Print("Type: Pop Group"); break;
        case GL_DEBUG_TYPE_OTHER: Console::Print("Type: Other"); break;
    
    }

    Console::PrintLine("");

    switch(_severity){
        case GL_DEBUG_SEVERITY_HIGH: Console::Print("Severity: high"); break;
        case GL_DEBUG_SEVERITY_MEDIUM: Console::Print("Severity: medium"); break;
        case GL_DEBUG_SEVERITY_LOW: Console::Print("Severity: low"); break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: Console::Print("Severity: notification"); break;
    }

    Console::PrintLine("");

}