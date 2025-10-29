#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include "../console/console.h"
#include <SDL3/SDL.h>
#include "../json/json_variant.h"
#include "program_state.h"
#include "../imgui/imgui.h"
#include "tab.h"
#include "main_menu_bar.h"

namespace UFOEngineStudio{

void TabBar(ProgramState* _program){

    ImGui::BeginTabBar("Opened Files");

    for(auto&& tab : _program->tabs){
        tab->Update(_program);
    }

    ImGui::EndTabBar();

    for(int tab_index = _program->tabs.size() -1; tab_index >= 0; tab_index--){
        if(!_program->tabs[tab_index]->opened){
            _program->tabs.erase(_program->tabs.begin()+tab_index);
        }
    }

}

}