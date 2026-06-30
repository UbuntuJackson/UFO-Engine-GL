#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../shapes/rectangle.h"
#include "rectangular_area.h"
#include "camera.h"

#ifdef UFO_ENGINE_STUDIO
#include "im_vec.h"
#include "viewport_editing_utils.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#endif

namespace ufo{

RectangularArea::RectangularArea(Vector2f _) : Actor(_){
    class_name = "ufo::RectangularArea";
    base_class_name = class_name;
    is_editor_hit_box_unique_per_instance = true;
}

ufo::Rectangle RectangularArea::GetRectangle(){
    return ufo::Rectangle(GetGlobalPosition()+rectangle.position,rectangle.size);
}

void RectangularArea::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
    //A good example of large amount of properties being written to an object
    // Potential solution, have an additional map which handles writing of default properties.
    // Other solution, pass json. I like this solution more, because that makes the generated code more managable.
    // Son of a biscuit this has been redundant.
    // Writing of custom properties handled in generated.h.

    try{
        auto j_rectangle = _json->map.at("rectangle")->AsMap();
        float rectangle_x = j_rectangle.at("x")->AsFloat();
        float rectangle_y = j_rectangle.at("y")->AsFloat();
        float rectangle_w = j_rectangle.at("w")->AsFloat();
        float rectangle_h = j_rectangle.at("h")->AsFloat();
        editor_hitbox.position.x = rectangle_x;
        editor_hitbox.position.y = rectangle_y;
        editor_hitbox.size.x = rectangle_w;
        editor_hitbox.size.y = rectangle_h;
        editor_hitbox = rectangle;
    } catch(const std::exception& _error){
        Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing ufo::RectangularArea instance");
    }
}

ufo::gc::JsonMap* RectangularArea::GetAsJson(ufo::GarbageCollector* _gc){

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    auto j_rectangle = _gc->New<ufo::gc::JsonMap>();
    j_rectangle->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.position.x));
    j_rectangle->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.position.y));
    j_rectangle->map.emplace("w", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.size.x));
    j_rectangle->map.emplace("h", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.size.y));

    parent_class_as_json->map.emplace("rectangle", j_rectangle);
    return parent_class_as_json;
}

#ifdef UFO_ENGINE_STUDIO

void RectangularArea::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    auto cam = _level_editor_tab->this_level->active_camera_handles.back();

    {
        Vector2f pos_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+editor_hitbox.position);
        Vector2f pos_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+editor_hitbox.position+editor_hitbox.size);
        Vector2f global_position = GetGlobalPosition();

        ImU32 colour = 0xFFFFFFFF;
        if(parent->base_class_name != "ufo::Level") colour = 0xFF664422;

        ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min),UFOEngineStudio::FromVector2fToImVec2(pos_max), colour);

    }

    ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

    Vector2f viewport_pos = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

    ImVec2 window_pos = ImGui::GetMainViewport()->Pos;

    Vector2f editor_viewport_pos = Vector2f(viewport_pos.x-window_pos.x,viewport_pos.y-window_pos.y);

    Vector2f scaled_delta_mouse_position = 1.0f/cam->scale * (engine->mouse.position - engine->mouse.former_position) * _level_editor_tab->window_to_engine_ratio;

    ufo::ResizeOrMove(this,
        part_of_rectangle_resized_in_editor,
        cam->Transform(GetGlobalPosition()+editor_hitbox.position), cam->scale* editor_hitbox.size,
        editor_hitbox.position, editor_hitbox.size,
        _level_editor_tab->mouse_position_over_screenspace,
        scaled_delta_mouse_position);

}

#endif

}
