/*

    This class is currently unused

*/

#pragma once
#include <memory>
#include <SDL3/SDL.h>
#include "../imgui/imgui.h"
#include "tab.h"
#include "actor_node.h"

namespace UFOEngineStudio{

class ProgramState;

class ActorComposerTab : public Tab{
public:

    std::unique_ptr<ActorNode> actor;

    ActorComposerTab(ProgramState* _program_state);

    void ConvertJsonToCPP(std::string& _header_file, std::vector<std::string>& _used_actor_classes, JsonDictionary* _json, ProgramState* _program_state);

    void OnActive(ImGuiID _local_dockspace_id, ProgramState* _program_state);

    void OnMakeDockSpace(ImGuiID _local_dockspace_id, ProgramState* _program_state);

    void OnSave(ProgramState* _program_state);

};

}