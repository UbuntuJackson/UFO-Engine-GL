#pragma once
#include <string>
#include "../imgui/imgui.h"
#include "../ufo_garbage_collector/object.h"

namespace ufo{class Graphics;}

namespace UFOEngineStudio{

class Editor;

class Tab : public ufo::gc::Root{
public:

    int id = 0;

    Editor* editor = nullptr;

    static inline int id_counter = 0;

    std::string name;
    std::string path;
    bool opened = true;
    bool is_edited = false;
    std::string name_and_imgui_id;
    Tab(Editor* _editor);

    virtual ~Tab() = default;
    virtual void Refresh();

    virtual void Update(Editor* _program_state, float _delta_time);
    virtual bool DetermineIfEdited();
    virtual void OnActive(ImGuiID _local_dockspace_id, Editor* _program_state, float _delta_time);
    virtual void OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _program_state);

    virtual void OnSave(Editor* _program_state);

    virtual void LevelUpdatePhase([[maybe_unused]] float _float){

    }

    virtual void LevelDrawPhase([[maybe_unused]] ufo::Graphics* _graphics){

    }

};

}
