#include <vector>
#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "graphics.h"
#include "actor.h"
#include "camera.h"
#include "widget.h"
#include "engine.h"
#include "sprite_utils.h"
#include "ufo_macros.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui.h"
#include "viewport_editing_utils.h"
#include "../ufo_engine_studio/im_vec.h"
#include "../ufo_engine_studio/file_dialogue.h"
#include "../ufo_engine_studio/editor.h"
#endif

namespace ufo{

Widget::Widget(Vector2f _) : Actor(_){
    class_name = "ufo::Widget";
    base_class_name = class_name;
    editor_hitbox = ufo::Rectangle(Vector2f(0.0f, 0.0f), Vector2f(32.0f, 32.0f));
}

ufo::Rectangle Widget::GetRectangle(){
    return ufo::Rectangle(GetGlobalPosition()+rectangle.position,rectangle.size);
}

void Widget::OnDraw(ufo::Graphics* _graphics, ufo::Camera* _camera){
    if(!visible) return;

    ufo::Rectangle sample_rectangle = SpriteUtils::GetFrameFromSpriteSheet(&engine->asset_manager,texture_key,current_frame_index,frame_size);

    _graphics->DrawPartialSprite(
        texture_key,
        _camera->Transform(GetGlobalPosition()-Vector2f(corner_rounding, corner_rounding)),
        offset,
        scale * _camera->scale,
        rectangle.position,
        rectangle.size+Vector2f(corner_rounding, corner_rounding)*2.0f,
        0,
        tint,
        shader_key,
        corner_rounding
    );
}

void Widget::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
    //A good example of large amount of properties being written to an object
    // Potential solution, have an additional map which handles writing of default properties.
    // Other solution, pass json. I like this solution more, because that makes the generated code more managable.
    // Son of a biscuit this has been redundant.
    // Writing of custom properties handled in generated.h.

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

            std::vector<std::string> texture_names;
            for(const auto& [name, texture] : engine->asset_manager.textures){
                bool search_is_in_word = false;

                for(int c = 0; c < (int)name.size(); c++){
                    bool found_match_from_this_character = true;

                    for(int d = 0; d < (int)_level_editor_tab->asset_browser_search.size(); d++){
                        if(c+d > (int)name.size()-1) continue;

                        if(_level_editor_tab->asset_browser_search[d]!=name[c+d]){
                            found_match_from_this_character = false;
                        }
                    }

                    if(found_match_from_this_character) search_is_in_word = true;
                }

                if(search_is_in_word) texture_names.push_back(name);
            }
            std::sort(texture_names.begin(), texture_names.end(), [](const std::string& _a,const std::string& _b){
                return _a<_b;
            });

            for(const std::string& name : texture_names){

                auto& texture = engine->asset_manager.textures.at(name);

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
                    ImGui::Text(texture.permanent ? "Status: Permanent" : "Status: Temporary");
                }

            }

            if(texture_was_erased && name_of_erased_texture != "placeholder_icon"){
                engine->asset_manager.textures.at(name_of_erased_texture).Delete();
                engine->asset_manager.textures.erase(name_of_erased_texture);
                _level_editor_tab->editor->ResourcesEdited();

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

            std::vector<std::string> shader_names;
            for(const auto& [name, shader] : engine->asset_manager.shaders){
                bool search_is_in_word = false;

                for(int c = 0; c < (int)name.size(); c++){
                    bool found_match_from_this_character = true;

                    for(int d = 0; d < (int)_level_editor_tab->asset_browser_search.size(); d++){
                        if(c+d > (int)name.size()-1) continue;

                        if(_level_editor_tab->asset_browser_search[d]!=name[c+d]){
                            found_match_from_this_character = false;
                        }
                    }

                    if(found_match_from_this_character) search_is_in_word = true;
                }

                if(search_is_in_word) shader_names.push_back(name);
            }
            std::sort(shader_names.begin(), shader_names.end(), [](const std::string& _a,const std::string& _b){
                return _a<_b;
            });

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
                _level_editor_tab->editor->ResourcesEdited();

                if(shader_key == name_of_erased_shader) shader_key = "partial_sprite_shader";

            }

            ImGui::EndChild();
        }


        ImGui::EndTabItem();

    }
}

void Widget::OnViewProperties([[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab, [[maybe_unused]] int _index){
    ImGui::InputFloat("corner_rounding", &corner_rounding);
}

void Widget::OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

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
        local_position, editor_hitbox.size,
        _level_editor_tab->mouse_position_over_screenspace,
        scaled_delta_mouse_position
    );

    rectangle = editor_hitbox;

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
