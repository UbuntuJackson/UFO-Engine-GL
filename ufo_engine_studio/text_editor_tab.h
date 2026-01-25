#pragma once
#include <string>
#include "tab.h"
#include  "../utils/console.h"
#include <memory>
#include "../src/actor.h"

namespace UFOEngineStudio{

class Editor;

class Action{
public:
    Actor* actor = nullptr;

    Action(){

    }

    virtual void Undo() = 0;
    virtual void Redo() = 0;
};

class AddAction : public Action{
public:

    void Undo(){

    }

    void Redo(){

    }
};

class RemoveAction : public Action{
public:

    void Undo(){

    }

    void Redo(){

    }
};

class EditAction : public Action{
public:

    void Undo(){

    }

    void Redo(){

    }
};



class TextEditorTab : public Tab{
public:
    std::string text;
    std::string last_saved_text;

    std::vector<std::unique_ptr<Action>> actions;

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
