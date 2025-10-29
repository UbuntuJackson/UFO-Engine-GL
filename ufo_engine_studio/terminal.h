#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "../console/console.h"
#include <SDL3/SDL.h>
#include "../json/json_variant.h"
#include "program_state.h"
#include "../imgui/imgui.h"

namespace UFOEngineStudio{

void Terminal(ProgramState* _program){
    
    /*std::stringstream text_stream;

    text_stream << (std::cout.rdbuf());
    static std::string text = "Terminal" + text_stream.str();
    ImGui::Text(text.c_str(),"");*/
}

}