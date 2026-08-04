#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../tilemap/tile_map.h"
#include "camera.h"
#include "graphics.h"
#include "platformer_rectangle_collision.h"
#include "engine.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/im_vec.h"
#include "viewport_editing_utils.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"
#endif

namespace ufo{

#ifdef UFO_ENGINE_STUDIO
void PlatformerRectangleCollision::OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    if(import_mode == ImportModes::CUSTOM_CLASS) return;

    auto cam = _level_editor_tab->this_level->active_camera_handles.back();

    {
        Vector2f pos_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+editor_hitbox.position);
        Vector2f pos_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+editor_hitbox.position+editor_hitbox.size);

        ImU32 colour = 0xFFFFFFFF;
        if(parent->base_class_name != "ufo::Level") colour = 0xFF664422;

        ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min),UFOEngineStudio::FromVector2fToImVec2(pos_max), colour);

    }

    ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

    Vector2f viewport_pos = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

    Vector2f scaled_delta_mouse_position = 1.0f/cam->scale * (engine->mouse.position - engine->mouse.former_position) * _level_editor_tab->window_to_engine_ratio;

    ufo::ResizeOrMove(this,
        part_of_rectangle_resized_in_editor,
        cam->Transform(GetGlobalPosition()+editor_hitbox.position), cam->scale* editor_hitbox.size,
        editor_hitbox.position, editor_hitbox.size,
        _level_editor_tab->mouse_position_over_screenspace,
        scaled_delta_mouse_position);

}
#endif //UFO_ENGINE_STUDIO

}
