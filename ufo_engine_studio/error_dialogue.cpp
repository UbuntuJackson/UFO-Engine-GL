#include <memory>
#include <string>
#include "console.h"
#include "level_editor_tab.h"
#include "text_editor_tab.h"
#include "file_dialogue.h"
#include "../utils/file_utils.h"
#include "../imgui/imgui_internal.h"
#include "error_dialogue.h"
#include "editor.h"
#include "ufo_maths.h"

namespace UFOEngineStudio{


void ErrorDialogueNoError::Update(Editor* _editor){

}

ErrorDialogueFailedToOpenFile::ErrorDialogueFailedToOpenFile(const std::string& _file):file_path{_file}{}

void ErrorDialogueFailedToOpenFile::Update(Editor* _editor){

    if(!initialised){
        initialised = true;
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x/2.0f,ImGui::GetWindowSize().y/2.0f));
        ImGui::SetNextWindowSize(ImVec2(250,150));
    }

    ImGui::Begin("Error");

    ImGui::TextWrapped("%s",(std::string("Had trouble to open file "+file_path+
        ".  For now, check if the file is up to standards with the current UFO-Engine Studio version.").c_str()));

    ImGui::Separator();

    if(ImGui::Button("Ok")){
        _editor->error_dialogue = std::make_unique<ErrorDialogueNoError>();
    }

    ImGui::End();

}

}
