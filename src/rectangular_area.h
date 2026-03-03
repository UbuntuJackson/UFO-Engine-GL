#pragma once

#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../shapes/rectangle.h"
#include "im_vec.h"
#include "viewport_editing_utils.h"
#include "../ufo_engine_studio/level_editor_tab.h"

namespace ufo{

class RectangularArea : public Actor{
public:
    RectangularArea(Vector2f _) : Actor(_){
        class_name = "ufo::RectangularArea";
        base_class_name = class_name;
    }

    ufo::Rectangle rectangle = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(100.0f, 50.0f));

    PartsOfRectangle part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

        Actor::OnUpdateEditorViewport(_editor, _level_editor_tab);

        auto cam = _level_editor_tab->this_level->active_camera_handles.back();

        {
            Vector2f pos_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+rectangle.position);
            Vector2f pos_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+rectangle.position+rectangle.size);
            Vector2f global_position = GetGlobalPosition();

            ImU32 colour = 0xFFFFFFFF;
            if(parent->base_class_name != "ufo::Level") colour = 0xFF664422;

            ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min),UFOEngineStudio::FromVector2fToImVec2(pos_max), colour);

        }

        ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

        Vector2f viewport_pos = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

        ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

        Vector2f editor_viewport_pos = Vector2f(viewport_pos.x-window_pos.x,viewport_pos.y-window_pos.y);

        ufo::ResizeOrMove(this,
            part_of_rectangle_resized_in_editor,
            cam->Transform(GetGlobalPosition()+rectangle.position), cam->scale* rectangle.size,
            rectangle.position, rectangle.size,
            _level_editor_tab->mouse_position_over_screenspace);


    }

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc){

        ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

        auto j_rectangle = _gc->New<ufo::gc::JsonMap>();
        j_rectangle->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(rectangle.position.x));
        j_rectangle->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(rectangle.position.y));
        j_rectangle->map.emplace("w", _gc->New<ufo::gc::JsonNumber>(rectangle.size.x));
        j_rectangle->map.emplace("h", _gc->New<ufo::gc::JsonNumber>(rectangle.size.y));

        parent_class_as_json->map.emplace("rectangle", j_rectangle);
        return parent_class_as_json;
    }
};

}
