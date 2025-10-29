#pragma once
#include <string>
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"

namespace UFOEngineStudio{

enum SplitDirections{
    HORIZONTAL,
    VERTICAL
};

inline void ImGuiDockSpaceFill(int _dock_space_id, ImVec2 _size, const std::string& _first_sub_window_name, bool _do_once = true){
    if(!ImGui::DockBuilderGetNode(_dock_space_id) || !_do_once){
        ImGui::DockBuilderRemoveNode(_dock_space_id);
        ImGui::DockBuilderAddNode(_dock_space_id, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(_dock_space_id,_size);

        ImGuiID the_copy_assignment_everyone_does_for_some_reason = _dock_space_id;

        ImGui::DockBuilderDockWindow(_first_sub_window_name.c_str(), the_copy_assignment_everyone_does_for_some_reason);
        ImGui::DockBuilderFinish(_dock_space_id);
    }
}

inline void ImGuiDockSpaceSplit(int _dock_space_id, ImVec2 _size, const std::string& _first_sub_window_name, const std::string& _second_sub_window_name, SplitDirections _split_direction, bool _do_once = true){
    if(!ImGui::DockBuilderGetNode(_dock_space_id) || !_do_once){
        switch(_split_direction){
            case SplitDirections::HORIZONTAL :
            {
                ImGui::DockBuilderRemoveNode(_dock_space_id);
                ImGui::DockBuilderAddNode(_dock_space_id, ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(_dock_space_id,_size);
                
                ImGuiID the_copy_assignment_everyone_does_for_some_reason = _dock_space_id;

                ImGuiID dock_id_file_tree_left = ImGui::DockBuilderSplitNode(the_copy_assignment_everyone_does_for_some_reason, ImGuiDir_Left, 0.5f, nullptr, &the_copy_assignment_everyone_does_for_some_reason);
                ImGuiID dock_id_file_tree_right = ImGui::DockBuilderSplitNode(the_copy_assignment_everyone_does_for_some_reason, ImGuiDir_Right, 0.5f, nullptr, &dock_id_file_tree_left);

                ImGui::DockBuilderDockWindow(_first_sub_window_name.c_str(), dock_id_file_tree_left);
                ImGui::DockBuilderDockWindow(_second_sub_window_name.c_str(), dock_id_file_tree_right);
                ImGui::DockBuilderFinish(_dock_space_id);
            }
            break;
            case SplitDirections::VERTICAL :
            {
                ImGui::DockBuilderRemoveNode(_dock_space_id);
                ImGui::DockBuilderAddNode(_dock_space_id, ImGuiDockNodeFlags_DockSpace);
                ImGui::DockBuilderSetNodeSize(_dock_space_id,_size);
                
                ImGuiID the_copy_assignment_everyone_does_for_some_reason = _dock_space_id;

                ImGuiID dock_id_file_tree_left = ImGui::DockBuilderSplitNode(the_copy_assignment_everyone_does_for_some_reason, ImGuiDir_Up, 0.5f, nullptr, &the_copy_assignment_everyone_does_for_some_reason);
                ImGuiID dock_id_file_tree_right = ImGui::DockBuilderSplitNode(the_copy_assignment_everyone_does_for_some_reason, ImGuiDir_Down, 0.5f, nullptr, &dock_id_file_tree_left);

                ImGui::DockBuilderDockWindow(_first_sub_window_name.c_str(), dock_id_file_tree_left);
                ImGui::DockBuilderDockWindow(_second_sub_window_name.c_str(), dock_id_file_tree_right);
                ImGui::DockBuilderFinish(_dock_space_id);
            }
            break;
        }
    }
}

}