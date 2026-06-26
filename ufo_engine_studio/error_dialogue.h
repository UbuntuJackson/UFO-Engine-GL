#pragma once
#include <string>
#include "level_editor_tab.h"
#include "text_editor_tab.h"
#include "file_dialogue.h"
#include "../utils/file_utils.h"
#include "../imgui/imgui_internal.h"

namespace UFOEngineStudio{

class Editor;

class ErrorDialogue{
public:
    virtual void Update(Editor* _editor) = 0;

    virtual ~ErrorDialogue() = default;
};

class ErrorDialogueNoError : public ErrorDialogue{
    void Update(Editor* _editor);
};

class ErrorDialogueFailedToOpenFile : public ErrorDialogue{
public:
    bool initialised = false;
    std::string file_path;

    ErrorDialogueFailedToOpenFile(const std::string& _file);

    void Update(Editor* _editor);
};

}
