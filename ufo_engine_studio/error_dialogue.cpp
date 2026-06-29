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

    ImGui::Begin("Error", nullptr, ImGuiWindowFlags_NoDocking);

    ImGui::TextWrapped("%s",(std::string("Had trouble to open file "+file_path+
        ".  For now, check if the file is up to standards with the current UFO-Engine Studio version.").c_str()));

    ImGui::Separator();

    if(ImGui::Button("Ok")){
        _editor->error_dialogue = std::make_unique<ErrorDialogueNoError>();
    }

    ImGui::End();

}

//

ErrorDialogueMismatchedClass::ErrorDialogueMismatchedClass(const std::string& _scene_file_path, const std::string& _scene_file_base_class_name, const std::string& _source_file_class_name, const std::string& _base_class_name):
    scene_file_path{_scene_file_path},
    scene_file_base_class_name{_scene_file_base_class_name},
    source_file_class_name{_source_file_class_name},
    base_class_name{_base_class_name}{}

void ErrorDialogueMismatchedClass::Update(Editor* _editor){

    if(!initialised){
        initialised = true;
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x/2.0f,ImGui::GetWindowSize().y/2.0f));
        ImGui::SetNextWindowSize(ImVec2(250,150));
    }

    ImGui::Begin("Error", nullptr, ImGuiWindowFlags_NoDocking);

    ImGui::TextWrapped("%s",(
        std::string(
            "Missmatch between .ason file class and C++ class. Is class "+
            scene_file_base_class_name+" in "+scene_file_path+", but is "+
            base_class_name+" in C++ class. Actors will be shown as "+scene_file_base_class_name+
            ". Please use the typedef "+ source_file_class_name+"_BaseClass" +" in generated_base_classes.h").c_str()
        )
    );

    ImGui::Separator();

    if(ImGui::Button("Ok")){
        _editor->error_dialogue = std::make_unique<ErrorDialogueNoError>();
    }

    ImGui::End();

}

//...
InheritsFromUnknownClass::InheritsFromUnknownClass(const std::string& _class_name, const std::string& _base_class_name):
class_name{_class_name},
base_class_name(_base_class_name)
{}

void InheritsFromUnknownClass::Update(Editor* _editor){

    if(!initialised){
        initialised = true;
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetWindowSize().x/2.0f,ImGui::GetWindowSize().y/2.0f));
        ImGui::SetNextWindowSize(ImVec2(250,150));
    }

    ImGui::Begin("Error", nullptr, ImGuiWindowFlags_NoDocking);

    ImGui::TextWrapped("%s",(
        std::string(
            "Class "+class_name+" inherits from unknown base class "+base_class_name+". Please check for spelling errors in respective sourcefile.").c_str()
        )
    );

    ImGui::Separator();

    if(ImGui::Button("Ok")){
        _editor->error_dialogue = std::make_unique<ErrorDialogueNoError>();
    }

    ImGui::End();

}

}
