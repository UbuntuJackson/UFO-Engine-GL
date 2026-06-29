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

class ErrorDialogueMismatchedClass : public ErrorDialogue{
public:
    bool initialised = false;
    std::string scene_file_path;
    std::string source_file_class_name;
    std::string base_class_name;
    std::string scene_file_base_class_name;

    ErrorDialogueMismatchedClass(const std::string& _scene_file_path, const std::string& _scene_file_base_class_name, const std::string& _source_file_class_name, const std::string& _base_class_name);

    void Update(Editor* _editor);
};

class InheritsFromUnknownClass : public ErrorDialogue{
public:
    bool initialised = false;
    std::string class_name;
    std::string base_class_name;

    InheritsFromUnknownClass(const std::string& _class_name, const std::string& _base_class_name);

    void Update(Editor* _editor);
};

}
