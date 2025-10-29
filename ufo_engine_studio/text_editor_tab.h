#pragma once
#include <string>
#include "tab.h"
#include  "../console/console.h"

namespace UFOEngineStudio{

class ProgramState;

class TextEditorTab : public Tab{
public:
    std::string text;
    std::string last_saved_text;

    TextEditorTab(std::string _file, std::string _text){
        text = _text;
        last_saved_text = text;
        name = _file.substr(_file.find_last_of("/")+1);
    }

    bool DetermineIfEdited();

    void OnActive(ImGuiID _local_dockspace_id , ProgramState* _program_state);
    void OnMakeDockSpace(ImGuiID _local_dockspace_id, ProgramState* _program_state);
    void OnSave(ProgramState* _program_state);
};

};