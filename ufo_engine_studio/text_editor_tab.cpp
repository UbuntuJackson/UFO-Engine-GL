#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "text_editor_tab.h"
#include "../utils/console.h"
#include "dock_utils.h"
#include "editor.h"
#include "tab.h"
#include "../utils/file_utils.h"
#include "file_dialogue.h"

namespace UFOEngineStudio{

int FilterTabs(ImGuiInputTextCallbackData* data)
{

    data->InsertChars(data->CursorPos,"    ");

    return 0;
}

bool TextEditorTab::DetermineIfEdited(){
    return text == last_saved_text;
}

void TextEditorTab::OnActive(ImGuiID _local_dockspace_id , Editor* _editor, float _delta_time){

    ImGui::Begin(std::string("TextEditorTabWindow"+name_and_imgui_id).c_str());

    //Console::PrintLine(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);

    ImGui::InputTextMultiline(std::string("InputTextMultiline"+name_and_imgui_id).c_str(), &text, ImVec2(ImGui::GetWindowSize().x-32,ImGui::GetWindowSize().y-32), ImGuiInputTextFlags_CallbackCompletion, FilterTabs);

    ImGui::End();
}

void TextEditorTab::OnMakeDockSpace(ImGuiID _local_dockspace_id, Editor* _editor){
    ImGuiDockSpaceFill(_local_dockspace_id, ImGui::GetWindowSize(), std::string("TextEditorTabWindow"+name_and_imgui_id).c_str());
}

void TextEditorTab::OnSave(Editor* _editor){
    //File will have no name if it isn't read or created with respect to file system
    if(name != ""){
        ufo::FileSystem::Write(path,text);

        _editor->refresh_entire_project = true;
    }
    else{
        const char* global_file_location = _editor->opened_directory_path.c_str();

        SDL_ShowSaveFileDialog(&OnNewTextFile , this, _editor->engine->window, nullptr, 0, global_file_location);
    }

    last_saved_text = text;

    _editor->refresh_entire_project = true;
    Refresh();
}

};
