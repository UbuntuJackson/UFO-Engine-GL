#include "../imgui/imgui.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#include "text_editor_tab.h"
#include "../console/console.h"
#include "dock_utils.h"
#include "program_state.h"

namespace UFOEngineStudio{

int FilterTabs(ImGuiInputTextCallbackData* data)
{
    
    data->InsertChars(data->CursorPos,"    ");
    
    return 0;
}

bool TextEditorTab::DetermineIfEdited(){
    return text == last_saved_text;
}

void TextEditorTab::OnActive(ImGuiID _local_dockspace_id , ProgramState* _program_state){

    ImGui::Begin("Text- and Code Editing");
    ImGui::InputTextMultiline((name+"###TextEditor"+path).c_str(), &text, ImVec2(ImGui::GetWindowSize()), ImGuiInputTextFlags_CallbackCompletion, FilterTabs);
    
    ImGui::End();
}

void TextEditorTab::OnMakeDockSpace(ImGuiID _local_dockspace_id, ProgramState* _program_state){
    ImGuiDockSpaceFill(_local_dockspace_id, ImGui::GetWindowSize(), "Text- and Code Editing");
}

void TextEditorTab::OnSave(ProgramState* _program_state){
    //File will have no name if it isn't read or created with respect to file system
    if(name != ""){
        File f;
        f.Insert(text);
        f.Write(path);

        last_saved_text = text;
        _program_state->should_search_working_directory_for_exposed_actor_classes = true;
    }
}

};