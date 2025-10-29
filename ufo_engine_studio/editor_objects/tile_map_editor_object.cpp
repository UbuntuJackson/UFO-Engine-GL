#include "tile_map_editor_object.h"
#include "../../imgui/imgui.h"
#include "../im_vec.h"

namespace UFOEngineStudio{

void TileMapEditorObject::Update(ProgramState* _program_state, ActorEditorObject* _parent_editor_object, Vector2f _parent_position){

    //The size of each tile
    int spacing_vertical_lines_along_x_axis = 16;
    int spacing_horizontal_lines_along_y_axis = 16;

    //Number of tiles in x- and y-axis
    int number_of_vertical_lines_along_x_axis = 50;
    int number_of_horizontal_lines_along_y_axis = 20;

    ImVec2 child_window_size = ImVec2(spacing_vertical_lines_along_x_axis * number_of_vertical_lines_along_x_axis, spacing_horizontal_lines_along_y_axis * number_of_horizontal_lines_along_y_axis);

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    for(int xx = 0; xx < number_of_vertical_lines_along_x_axis; xx++){
        draw_list->AddLine(
            UFOEngineStudio::ImAdd(ImGui::GetWindowPos(), ImVec2(xx * spacing_vertical_lines_along_x_axis, 0)),
            UFOEngineStudio::ImAdd(ImGui::GetWindowPos(), ImVec2(xx * spacing_vertical_lines_along_x_axis, spacing_horizontal_lines_along_y_axis * number_of_horizontal_lines_along_y_axis)),
            ImColor(255,255,255,255) ,
            1.0f
        );
    }
    for(int yy = 0; yy < number_of_horizontal_lines_along_y_axis; yy++){
        draw_list->AddLine(
            UFOEngineStudio::ImAdd(ImGui::GetWindowPos(), ImVec2(0,                                                                           yy * spacing_horizontal_lines_along_y_axis)),
            UFOEngineStudio::ImAdd(ImGui::GetWindowPos(), ImVec2(spacing_vertical_lines_along_x_axis * number_of_vertical_lines_along_x_axis, yy * spacing_horizontal_lines_along_y_axis)),
            ImColor(255,255,255,255) ,
            1.0f
        );
    }
        
}

}