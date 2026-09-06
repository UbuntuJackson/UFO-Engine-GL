#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "frame_buffer_texture.h"
#include "graphics.h"
#include "rectangle.h"

#ifdef UFO_ENGINE_STUDIO
#include "viewport_editing_utils.h"
namespace UFOEngineStudio{
    class Editor;
    class LevelEditorTab;
}
#endif

namespace ufo{

class Camera;
class Graphics;

class Widget : public Actor{
public:
    enum ContentsLayoutMode{
        HORIZONTAL_LIST,
        VERTICAL_LIST,
        FREE_STYLE
    };
    enum ContentsResizeMode{
        DISTRIBUTE_SIZE_EQUALLY,
        DO_NOT_RESIZE
    };

    ContentsLayoutMode contents_layout_mode = VERTICAL_LIST;
    ContentsResizeMode contents_resize_mode = DO_NOT_RESIZE;
    int item_spacing = 3;

    int padding = 5;

    bool use_nine_patch_rectangle = false;
    int nine_patch_rect_left_bound = 3;
    int nine_patch_rect_right_bound = 4;
    int nine_patch_rect_lower_bound = 3;
    int nine_patch_rect_upper_bound = 4;

    bool visible = true;
    std::string texture_key = "placeholder_icon";
    Vector2f offset;
    float current_frame_index = 0;
    float number_of_frames = 1;
    Vector2f frame_size = {32.0f, 32.0f};
    Vector2f scale = {1.0f, 1.0f};
    ufo::Colour tint = ufo::Colour(255,255,255,255);
    std::string shader_key = "partial_sprite_shader";
    float corner_rounding = 0.0f;

    bool has_header = false;
    int header_height = 16;

    //For how long you've scrolled, if scollbar is triggered.
    float scroll_x = 0.0f;
    float scroll_y = 0.0f;
    float contents_to_window_ratio_x = 1.0f;
    float contents_to_window_ratio_y = 1.0f;
    std::string scroll_bar_texture_key = "white_square";
    float vertical_scroll_bar_width = 16.0f;
    float horizontal_scroll_bar_height = 16.0f;
    float scroll_bar_corner_counding = 4.0f;
    std::string scroll_bar_shader_key = "partial_sprite_shader";
    ufo::Colour scroll_bar_tint = ufo::Colour(0,0,0,255);
    bool IsHorizontalScrollBarActive();
    bool IsVerticalScrollBarActive();
    float GetHorizontalFreeSpace();
    float GetVerticalFreeSpace();

    bool is_vertical_scroll_bar_held = false;
    bool is_horizontal_scroll_bar_held = false;

    FrameBufferTexture frame_buffer_texture;

    Widget(Vector2f _);

    void OnSpawn() override;

    ufo::Rectangle GetLocalRectangle() override;
    ufo::Rectangle GetRectangle() override;

    void OnIrregularUpdate() override;
    void UpdateContentLayoutAndSize();
    void Update(float _delta_time) override;
    bool IsHorizontalScrollBarHovered();
    bool IsVerticalScrollBarHovered();
    bool ClickableArea() override;
    void OnClickableArea() override;

    void OnDraw(ufo::Graphics* _graphics, ufo::Camera* _camera) override;
    void Draw(ufo::Graphics *_graphics, ufo::Camera *_camera) override;
    virtual void DrawUnscaled(ufo::Graphics *_graphics, ufo::Camera *_camera);
    void DrawFlattenWidgetTexture(ufo::Graphics *_graphics, FrameBufferTexture& _texture, ufo::Widget* _parent) override;
    FrameBufferTexture FlattenWidgetTextures(ufo::Graphics *_graphics, ufo::Camera *_camera, ufo::Widget *_parent, unsigned int _former_frame_buffer_object, Vector2f _former_frame_buffer_size, Vector2f _former_frame_buffer_projection_min,Vector2f _former_frame_buffer_projection_max) override;
    void OnWidgetDraw(ufo::Graphics *_graphics, ufo::Camera *_camera) override;

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json) override;
    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;

    #ifdef UFO_ENGINE_STUDIO
    bool IsMovable() override;
    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    PartsOfRectangle part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;
    void OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnResourcesEdited() override;

    #endif
};

}
