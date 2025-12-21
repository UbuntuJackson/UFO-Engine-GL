#pragma once
#include <string>
#include "tab.h"
#include  "../utils/console.h"

namespace UFOEngineStudio{

class Editor;

class TextEditorTab : public Tab{
public:
    std::string text;
    std::string last_saved_text;

    TextEditorTab(std::string _file, std::string _text, Editor* _editor) : Tab(_editor){
        text = _text;
        last_saved_text = text;
        name = _file.substr(_file.find_last_of("/")+1);
    }

    bool DetermineIfEdited();

    void OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time);
    void OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _editor);
    void OnSave(Editor* _editor);
};

};
