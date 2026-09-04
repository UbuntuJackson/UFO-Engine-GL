#include <string>
#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "console.h"
#include "frame_buffer_texture.h"
#include "graphics.h"
#include "actor.h"
#include "camera.h"
#include "widget.h"
#include "engine.h"
#include "rectangle.h"
#include "sprite_utils.h"
#include "texture_2d.h"
#include "ufo_macros.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include "viewport_editing_utils.h"
#include "../ufo_engine_studio/im_vec.h"
#include "../ufo_engine_studio/file_dialogue.h"
#include "../ufo_engine_studio/editor.h"
#include "../ufo_engine_studio/imgui_utils.h"
#endif

namespace ufo{

Widget::Widget(Vector2f _) : Actor(_){
    class_name = "ufo::Widget";
    base_class_name = class_name;
    editor_hitbox = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(32.0f, 32.0f));
}

void Widget::OnSpawn(){
    IrregularUpdate();
}

ufo::Rectangle Widget::GetLocalRectangle(){

    if(!has_header) return rectangle;

    return ufo::Rectangle(Vector2f(rectangle.position.x, rectangle.position.y+(float)header_height),rectangle.size-Vector2f(0.0f, (float)header_height));
}

void Widget::OnIrregularUpdate(){
    UpdateContentLayoutAndSize();
    editor_hitbox = rectangle;
}

void Widget::Update(float _delta_time){
    ClickableArea();
}

ufo::Rectangle Widget::GetRectangle(){
    return ufo::Rectangle(GetGlobalPosition()+rectangle.position, rectangle.size);
}

bool Widget::ClickableArea(){

    Vector2f mouse_position = engine->mouse.position;
#ifdef UFO_ENGINE_STUDIO
    auto level_editor_tab = dynamic_cast<UFOEngineStudio::LevelEditorTab*>(engine->editor.active_tab);
    mouse_position = level_editor_tab->mouse_position_over_screenspace;
#endif

    if(ufo::Maths::RectangleVsPoint(GetRectangle(), level->active_camera_handles.back()->TransformScreenToWorld(mouse_position))){

        for(const auto& actor : actors){
            if(actor->ClickableArea()){
                return true;
            }

        }

        OnClickableArea();
        return true;
    }

    return false;
}

void Widget::OnClickableArea(){
    Console::PrintLine("OnClickableArea",editor_name);
}

void Widget::UpdateContentLayoutAndSize(){
    if(contents_layout_mode == ContentsLayoutMode::HORIZONTAL_LIST){
        if(contents_resize_mode == ContentsResizeMode::DO_NOT_RESIZE){

            float local_horizontal_position_for_widget_column = 0.0f;
            float height_incrementation = 0.0f;

            for(int c = 0; c < actors.size(); c++){
                Actor* act = actors[c].get();

                act->local_position.x = local_horizontal_position_for_widget_column;
                act->local_position.y = height_incrementation;
                act->rectangle.position = Vector2f(0.0f, 0.0f);
                height_incrementation+=act->rectangle.size.y;
            }

            contents_to_window_ratio_y = rectangle.size.y/height_incrementation;
            if(contents_to_window_ratio_y < 1.0f){
                for(int c = 0; c < actors.size(); c++){
                    Actor* act = actors[c].get();
                    act->local_position.y-=((height_incrementation-rectangle.size.y) * scroll_y);
                }
            }
            else{
                scroll_y = 0.0f;
            }
        }
        if(contents_resize_mode == ContentsResizeMode::DISTRIBUTE_SIZE_EQUALLY){
            float local_horizontal_position_for_widget_column = 0.0f;
            float height_incrementation = 0.0f;

            for(int c = 0; c < actors.size(); c++){
                Actor* act = actors[c].get();

                act->local_position.x = local_horizontal_position_for_widget_column;
                act->local_position.y = c*rectangle.size.y/(float)actors.size();
                act->rectangle.position = Vector2f(0.0f, 0.0f);
                act->rectangle.size.y = rectangle.size.y/(float)actors.size();
            }
        }
    }
}

void Widget::OnDraw(ufo::Graphics* _graphics, ufo::Camera* _camera){
    if(!visible) return;

    ufo::Rectangle sample_rectangle = SpriteUtils::GetFrameFromSpriteSheet(&engine->asset_manager,texture_key,current_frame_index,frame_size);

    _graphics->DrawPartialSprite(
        texture_key,
        _camera->Transform(GetGlobalPosition()),
        offset,
        scale * _camera->scale,
        rectangle.position,
        rectangle.size,
        0,
        tint,
        shader_key,
        corner_rounding
    );
}

void Widget::Draw(ufo::Graphics *_graphics, ufo::Camera *_camera){
    //OnDraw(_graphics, _camera);
    //CreateWidgetTexture(_graphics, _camera, parent->DynamicCast<ufo::Widget>(), _graphics->GetFrameBufferObject(), Vector2f(engine->game_width, engine->game_height),Vector2f(0.0f, engine->game_height),Vector2f(engine->game_width, 0.0f));
    OnWidgetDraw(_graphics, _camera);

}

void Widget::OnWidgetDraw(ufo::Graphics *_graphics, ufo::Camera *_camera){
    FrameBufferTexture flattened_tex = FlattenWidgetTextures(
        _graphics,
        _camera,
        this,
        _graphics->GetFrameBufferObject(),
        Vector2f(engine->game_width, engine->game_height), //Size
        Vector2f(0.0f, engine->game_height), //Projection min
        Vector2f(engine->game_width, 0.0f) //Projection max
    );

    _graphics->DrawPartialSprite(
        flattened_tex,
        _camera->Transform(GetGlobalPosition()),
        Vector2f(0.0f, 0.0f),
        scale * _camera->scale,
        Vector2f(0.0f, 0.0f),
        Vector2f(flattened_tex.width,flattened_tex.height),
        0,
        tint,
        shader_key,
        corner_rounding
    );

    glDeleteTextures(1, &flattened_tex.id);
}

void Widget::DrawFlattenWidgetTexture(ufo::Graphics *_graphics, FrameBufferTexture& _texture, ufo::Widget* _parent){

    _graphics->DrawPartialSprite(
        _texture,
        local_position,
        offset,
        scale,
        rectangle.position,
        rectangle.size,
        0,
        tint,
        shader_key,
        corner_rounding
    );
}

FrameBufferTexture Widget::FlattenWidgetTextures(ufo::Graphics *_graphics, ufo::Camera *_camera, ufo::Widget *_parent, unsigned int _former_frame_buffer_object, Vector2f _former_frame_buffer_size, Vector2f _former_frame_buffer_projection_min,Vector2f _former_frame_buffer_projection_max){

    FrameBufferTexture local_frame_buffer_texture;

    bool failed_to_create_frame_buffer_object = false;

    Vector2f total_size = rectangle.size;

    unsigned int local_frame_buffer_object = local_frame_buffer_texture.CreateFrameBuffer(total_size.x, total_size.y, failed_to_create_frame_buffer_object);
    local_frame_buffer_texture.BindFrameBuffer(local_frame_buffer_object);

    //Set projection to draw on local frame buffer
    glViewport(0,0,local_frame_buffer_texture.width, local_frame_buffer_texture.height);
    _graphics->SetProjection(0.0f, local_frame_buffer_texture.width, 0.0f, local_frame_buffer_texture.height);

    _graphics->DrawPartialSprite(
        texture_key,
        //Relative coordinate for speite will always be 0, because the framebuffer is the same dimensions as the texture of this widget
        Vector2f(0.0f, 0.0f),
        offset,
        scale,
        rectangle.position,
        rectangle.size,
        0,
        tint,
        shader_key,
        corner_rounding
    );

    if(contents_to_window_ratio_y < 1.0f) _graphics->DrawPartialSprite(
        scroll_bar_texture_key,
        Vector2f(rectangle.size.x-scroll_bar_thickness, 0.0f),
        Vector2f(0.0f, 0.0f),
        Vector2f(1.0f, 1.0f),
        Vector2f(0.0f, 0.0f),
        Vector2f(scroll_bar_thickness, rectangle.size.y),
        0.0f,
        scroll_bar_tint,
        scroll_bar_shader_key,
        scroll_bar_corner_counding
    );

    for(const auto& actor : actors){

        FrameBufferTexture tex = actor->FlattenWidgetTextures(_graphics, _camera, this, local_frame_buffer_object, Vector2f(local_frame_buffer_texture.width, local_frame_buffer_texture.height), Vector2f(0.0f,0.0f),Vector2f(local_frame_buffer_texture.width,local_frame_buffer_texture.height));

        actor->DrawFlattenWidgetTexture(_graphics, tex, _parent);

        glDeleteTextures(1, &tex.id);
    }

    //DrawFlattenWidgetTexture(_graphics, frame_buffer_texture, _parent);

    glBindFramebuffer(GL_FRAMEBUFFER, _former_frame_buffer_object);
    glViewport(0,0,_former_frame_buffer_size.x, _former_frame_buffer_size.y);
    _graphics->SetProjection(_former_frame_buffer_projection_min.x, _former_frame_buffer_projection_max.x,_former_frame_buffer_projection_min.y, _former_frame_buffer_projection_max.y);
    if(!failed_to_create_frame_buffer_object) glDeleteFramebuffers(1, &local_frame_buffer_object);

    return local_frame_buffer_texture;

}

void Widget::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
    //A good example of large amount of properties being written to an object
    // Potential solution, have an additional map which handles writing of default properties.
    // Other solution, pass json. I like this solution more, because that makes the generated code more managable.
    // Son of a biscuit this has been redundant.
    // Writing of custom properties handled in generated.h.

    Actor::OnLoadDefaultProperties(_json);

    try{
        auto j_rectangle = _json->map.at("rectangle")->AsMap();
        float widget_x = j_rectangle.at("x")->AsFloat();
        float widget_y = j_rectangle.at("y")->AsFloat();
        float widget_w = j_rectangle.at("w")->AsFloat();
        float widget_h = j_rectangle.at("h")->AsFloat();
        rectangle.position.x = widget_x;
        rectangle.position.y = widget_y;
        rectangle.size.x = widget_w;
        rectangle.size.y = widget_h;

        if(import_mode != ImportModes::CUSTOM_CLASS){
            editor_hitbox = rectangle;
        }

    } catch(const std::exception& _error){
        Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing ufo::Widget instance");
    }

    _json->TryToGetValueAsFloat("corner_rounding", corner_rounding, GetInfo()+" "+__UFO_PRETTY_FUNCTION__);
    _json->TryToGetValueAsString("texture_key", texture_key, GetInfo()+" "+__UFO_PRETTY_FUNCTION__);
}

ufo::gc::JsonMap* Widget::GetAsJson(ufo::GarbageCollector* _gc){

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    auto j_rectangle = _gc->New<ufo::gc::JsonMap>();
    j_rectangle->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(rectangle.position.x));
    j_rectangle->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(rectangle.position.y));
    j_rectangle->map.emplace("w", _gc->New<ufo::gc::JsonNumber>(rectangle.size.x));
    j_rectangle->map.emplace("h", _gc->New<ufo::gc::JsonNumber>(rectangle.size.y));

    parent_class_as_json->map.emplace("texture_key", _gc->New<ufo::gc::JsonString>(texture_key));
    parent_class_as_json->map.emplace("rectangle", j_rectangle);
    parent_class_as_json->map.emplace("corner_rounding", _gc->New<ufo::gc::JsonNumber>(corner_rounding));

    return parent_class_as_json;
}

#ifdef UFO_ENGINE_STUDIO

bool Widget::IsMovable(){
    if(!parent) return true;
    Widget* widget_parent = parent->DynamicCast<Widget>();
    if(!widget_parent) return true;
    return widget_parent->contents_layout_mode == ContentsLayoutMode::FREE_STYLE;
}

void Widget::OnDrawGizmos([[maybe_unused]] ufo::Graphics* _graphics, [[maybe_unused]] Camera* _camera, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Widget::OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(ImGui::BeginTabItem("Textures")){

        if(ImGui::Button("[+] Add Texture")){
            SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTexture, _level_editor_tab, engine->window, UFOEngineStudio::global_texture_filters, 2, _level_editor_tab->editor->opened_directory_path.c_str(), true);
        }

        if(ImGui::InputText("Search###SearchAssetBrowser", &_level_editor_tab->asset_browser_search)){

        }

        ImGui::Separator();

        if(ImGui::BeginChild("MyAssetsChildWindow")){

            bool texture_was_erased = false;
            std::string name_of_erased_texture = "";

            std::vector<std::string> texture_names = engine->asset_manager.SearchForAsset(engine->asset_manager.textures, _level_editor_tab->asset_browser_search);

            for(const std::string& name : texture_names){

                auto& texture = engine->asset_manager.textures.at(name);

                bool is_available_in_asset_browser = UFOEngineStudio::IsTextureAvailableInAssetBrowser(_level_editor_tab, level, name, texture);
                if(!is_available_in_asset_browser) continue;

                float w = (float)texture.width;
                float h = (float)texture.height;

                bool view_asset_details = ImGui::CollapsingHeader(std::string("###view_asset_details"+name).c_str(), nullptr, ImGuiTreeNodeFlags_SpanTextWidth);

                ImGui::SameLine();

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 1.0f));
                ImGui::ImageButton(name.c_str(),
                    (void*)(intptr_t)texture.id,
                    ImVec2(32.0f*w/h, 32.0f),
                    ImVec2(0,0),
                    ImVec2(1,1),
                    ImVec4(0,0,0,1)
                );
                ImGui::PopStyleVar();

                if(ImGui::IsItemHovered()) ImGui::SetTooltip(name.c_str(), "%s");

                if(view_asset_details){
                    if(ImGui::Button(std::string("Unload Texture###UnloadTexture"+name).c_str())){
                        name_of_erased_texture = name;
                        texture_was_erased = true;
                    }
                    ImGui::SameLine();
                    if(ImGui::Button(std::string("Assign Texture to Current Sprite###AddCostume"+name).c_str())){
                        texture_key = name;
                        frame_size = Vector2f(w,h);
                        number_of_frames = 1;

                    }
                    ImGui::Text(std::string("width: " + std::to_string(w) + " height: "+std::to_string(h)).c_str(),"%s");
                    ImGui::Text(("name: "+name).c_str(),"%s");
                    UFOEngineStudio::TextureSavabilityAndAvailabilityDetails(engine, level, name, texture);
                }

            }

            if(texture_was_erased && name_of_erased_texture != "placeholder_icon"){
                engine->asset_manager.textures.at(name_of_erased_texture).Delete();
                engine->asset_manager.textures.erase(name_of_erased_texture);
                for(const auto& loaded_level : engine->loaded_levels_for_editor) loaded_level->ResourcesEdited();

                if(texture_key == name_of_erased_texture) texture_key = "placeholder_icon";

            }

            ImGui::EndChild();

        }

        ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("Shaders")){

        if(ImGui::Button("[+] Add Shader")){
            SDL_ShowOpenFolderDialog(&UFOEngineStudio::OnOpenShader, _level_editor_tab, engine->window, _level_editor_tab->editor->opened_directory_path.c_str(), true);
        }

        if(ImGui::InputText("Search###SearchShaders", &_level_editor_tab->asset_browser_search)){

        }

        if(ImGui::BeginChild("MyShaders")){

            bool shader_was_erased = false;
            std::string name_of_erased_shader = "";

            std::vector<std::string> shader_names = engine->asset_manager.SearchForAsset(engine->asset_manager.textures, _level_editor_tab->asset_browser_search);

            for(const std::string& name : shader_names){

                bool view_asset_details = ImGui::CollapsingHeader(std::string(("name: "+name)+"###view_asset_details"+name).c_str(), nullptr, ImGuiTreeNodeFlags_SpanTextWidth);

                if(ImGui::IsItemHovered()) ImGui::SetTooltip(name.c_str(), "%s");

                if(view_asset_details){
                    if(ImGui::Button(std::string("Unload Shader###UnloadShader"+name).c_str())){
                        name_of_erased_shader = name;
                        shader_was_erased = true;
                    }
                    ImGui::SameLine();
                    if(ImGui::Button(std::string("Assign Shader to Current Sprite###AddCostume"+name).c_str())){
                        shader_key = name;

                    }

                }

            }

            if(shader_was_erased && name_of_erased_shader != "partial_sprite_shader"){
                engine->asset_manager.shaders.at(name_of_erased_shader).Delete();
                engine->asset_manager.shaders.erase(name_of_erased_shader);
                for(const auto& loaded_level : engine->loaded_levels_for_editor) loaded_level->ResourcesEdited();

                if(shader_key == name_of_erased_shader) shader_key = "partial_sprite_shader";

            }

            ImGui::EndChild();
        }


        ImGui::EndTabItem();

    }
}

void Widget::OnViewProperties([[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab, [[maybe_unused]] int _index){
    if(ImGui::InputFloat("scroll_y", &scroll_y)){
        level->IrregularUpdate();
    }

    ImGui::InputFloat("corner_rounding", &corner_rounding);
    ImGui::Checkbox("Has header",&has_header);

    std::string contents_resize_mode_preview_values[] = {"Destribute Size Equally","Do Not Resize"};

    if(ImGui::BeginCombo(std::string("Contens Resize Mode###Contens Resize Mode"+std::to_string(editor_id)).c_str(), contents_resize_mode_preview_values[contents_resize_mode].c_str())){
        for(int mode_index = 0; mode_index < 2; mode_index++){
            bool is_selected = ImGui::Selectable(contents_resize_mode_preview_values[mode_index].c_str());
            if(is_selected){
                contents_resize_mode = (ContentsResizeMode)mode_index;
                IrregularUpdate();
            }
        }

        ImGui::EndCombo();
    }
}

void Widget::OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    if(import_mode == ImportModes::CUSTOM_CLASS) return;

    auto cam = _level_editor_tab->this_level->active_camera_handles.back();

    {
        Vector2f pos_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+rectangle.position);
        Vector2f pos_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+rectangle.position+rectangle.size);

        ImU32 colour = 0xFFFFFFFF;
        if(parent->base_class_name != "ufo::Level") colour = 0xFF664422;

        ImGui::GetWindowDrawList()->AddRect(UFOEngineStudio::FromVector2fToImVec2(pos_min),UFOEngineStudio::FromVector2fToImVec2(pos_max), colour);

    }

    ImVec2 im_viewport_pos = ImGui::GetItemRectMin();

    Vector2f viewport_pos = Vector2f(im_viewport_pos.x, im_viewport_pos.y);

    Vector2f scaled_delta_mouse_position = 1.0f/cam->scale * (engine->mouse.position - engine->mouse.former_position) * _level_editor_tab->window_to_engine_ratio;

    ufo::ResizeOrMove(this,
        part_of_rectangle_resized_in_editor,
        cam->Transform(GetGlobalPosition()+rectangle.position), cam->scale* rectangle.size,
        local_position, rectangle.size,
        _level_editor_tab->mouse_position_over_screenspace,
        scaled_delta_mouse_position
    );

    level->IrregularUpdate();

    editor_hitbox = rectangle;

}

void Widget::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){



}

void Widget::OnResourcesEdited(){
    if(!engine->asset_manager.textures.count(texture_key)){
        texture_key = "placeholder_icon";
    }
    if(!engine->asset_manager.shaders.count(shader_key)){
        shader_key = "partial_sprite_shader";
    }
}

#endif //UFO_ENGINE_STUDIO

}
