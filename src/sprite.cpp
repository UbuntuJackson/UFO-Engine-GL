#include <stdexcept>
#include <string>
#include "../ufo_maths/ufo_maths.h"
#include "actor.h"
#include "engine.h"
#include "camera.h"
#include "../../shapes/rectangle.h"
#include "sprite.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "graphics.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"
#include "../ufo_engine_studio/file_dialogue.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#endif //UFO_ENGINE_STUDIO

namespace ufo{

Sprite::Sprite(olc::vf2d _position, std::string _key, olc::vf2d _offset, olc::vf2d _frame_size, olc::vf2d _scale, float _rotation, float _frame_index) :
Actor(_position),
key{_key},
offset{_offset},
frame_size{_frame_size},
scale{_scale},
rotation{_rotation},
current_frame_index{_frame_index}
{
    class_name = "ufo::Sprite";
    base_class_name = class_name;
}

Sprite::Sprite(olc::vf2d _position) :
Actor(_position),
key{"placeholder_icon"},
offset{Vector2f(0.0f,0.0f)},
frame_size{Vector2f(32.0f,32.0f)},
scale{Vector2f(1.0f,1.0f)},
rotation{0.0f},
current_frame_index{0.0f}
{
    class_name = "ufo::Sprite";
    base_class_name = class_name;
}

void Sprite::OnSpawn(){

    if(!engine->asset_manager.textures.count(key)){
        key = "placeholder_icon";
    }
    if(!engine->asset_manager.shaders.count(shader_key)){
        shader_key = "partial_sprite_shader";
    }
    //if(frame_size.x > engine->asset_manager.at(key).w) frame_size.x = engine->asset_manager.at(key).w;
    //if(frame_size.y > engine->asset_manager.at(key).h) frame_size.y = engine->asset_manager.at(key).h;
}

ufo::Rectangle
Sprite::GetRectangleFromPositionAndFrameSize(int _x, int _y, Vector2f _frame_size){
    ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
    return rect;
}

ufo::Rectangle
Sprite::GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size){
    int fx = 1;
    int fy = 1;
    //To be fixed, clamp the values for height and width
    if(_frame_size.x > 0.0f && _frame_size.y > 0.0f &&
        _frame_size.x <= engine->asset_manager.textures.at(_sprite_key).width && frame_size.y <= engine->asset_manager.textures.at(_sprite_key).height){
        fx = (int)_frame % (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x); //1 can only give me x = 0
        fy = (int)_frame / (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x);
    }
    return GetRectangleFromPositionAndFrameSize(
        fx,
        fy,
        _frame_size); //1 can only give y = 1
}

void Sprite::OnDraw(ufo::Graphics* _graphics, Camera* _camera){
    if(!visible) return;

    ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(key,current_frame_index,frame_size);
    _graphics->DrawPartialSprite(
        key,
        _camera->Transform(GetGlobalPosition()),
        /*size,*/
        offset,
        scale*_camera->scale,
        sample_rectangle.position,
        sample_rectangle.size,
        rotation,
        tint,
        shader_key
    );
}

ufo::gc::JsonMap* Sprite::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

    if(import_mode == WRAPPED) return parent_class_as_json;

    //These properties don't need to be stored if this actor's import_mode is == ImportModes::WRAPPED.
    // However they need to be recovered.

    //I'm gonna make it so all default properties are excluded if the object is wrapped

    //if(import_mode == ImportModes::UNWRAPPED){
    parent_class_as_json->map.emplace("key", _gc->New<ufo::gc::JsonString>(key));
    parent_class_as_json->map.emplace("offset_x", _gc->New<ufo::gc::JsonNumber>(offset.x));
    parent_class_as_json->map.emplace("offset_y", _gc->New<ufo::gc::JsonNumber>(offset.y));
    parent_class_as_json->map.emplace("frame_size_x", _gc->New<ufo::gc::JsonNumber>(frame_size.x));
    parent_class_as_json->map.emplace("frame_size_y", _gc->New<ufo::gc::JsonNumber>(frame_size.y));
    parent_class_as_json->map.emplace("scale_x", _gc->New<ufo::gc::JsonNumber>(scale.x));
    parent_class_as_json->map.emplace("scale_y", _gc->New<ufo::gc::JsonNumber>(scale.y));
    parent_class_as_json->map.emplace("rotation", _gc->New<ufo::gc::JsonNumber>(rotation));
    parent_class_as_json->map.emplace("frame_index", _gc->New<ufo::gc::JsonNumber>(current_frame_index));
    parent_class_as_json->map.emplace("shader_key", _gc->New<ufo::gc::JsonString>(shader_key));

    ufo::gc::JsonArray* j_colour = _gc->New<ufo::gc::JsonArray>();
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.r));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.g));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.b));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.a));

    parent_class_as_json->map.emplace("tint", j_colour);

    //}

    return parent_class_as_json;
}

void Sprite::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
    //A good example of large amount of properties being written to an object
    // Potential solution, have an additional map which handles writing of default properties.
    // Other solution, pass json. I like this solution more, because that makes the generated code more managable.
    // Son of a biscuit this has been redundant.
    // Writing of custom properties handled in generated.h.

    //if(import_mode == Actor::ImportModes::UNWRAPPED){

    try{
        key = _json->map.at("key")->AsString();
        offset.x = _json->map.at("offset_x")->AsFloat();
        offset.y = _json->map.at("offset_y")->AsFloat();
        frame_size.x = _json->map.at("frame_size_x")->AsFloat();
        frame_size.y = _json->map.at("frame_size_y")->AsFloat();
        scale.x = _json->map.at("scale_x")->AsFloat();
        scale.y = _json->map.at("scale_y")->AsFloat();
        rotation = _json->map.at("rotation")->AsFloat();
        current_frame_index = (float)_json->map.at("frame_index")->AsFloat();

        _json->TryToGetValueAsString("shader_key", shader_key);

        std::vector<gc::Json *> j_colour;
        _json->TryToGetValueAsArray("tint", j_colour);
        if((int)j_colour.size() == 4){
            float red = j_colour[0]->AsFloat();
            float green = j_colour[1]->AsFloat();
            float blue = j_colour[2]->AsFloat();
            float alpha = j_colour[3]->AsFloat();
            tint = ufo::Colour(red, green, blue, alpha);
#ifdef UFO_ENGINE_STUDIO
            im_colour = ImVec4(red/255.0f,green/255.0f,blue/255.0f,alpha/255.0f);
#endif
        }

    } catch(const std::exception& _error){
        Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for json representing Sprite instance");
    }

    //}
}

#ifdef UFO_ENGINE_STUDIO

void Sprite::OnDrawGizmos([[maybe_unused]] ufo::Graphics* _graphics, [[maybe_unused]] Camera* _camera, [[maybe_unused]] UFOEngineStudio::LevelEditorTab* _level_editor_tab){

}

void Sprite::OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(ImGui::BeginTabItem("Textures")){

        if(ImGui::Button("[+] Add Texture")){
            SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenTexture, _level_editor_tab, engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), true);
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
                        float w = (float)engine->asset_manager.textures.at(name).width;
                        float h = (float)engine->asset_manager.textures.at(name).height;
                        key = name;
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
                _level_editor_tab->this_level->ResourcesEdited();

                if(key == name_of_erased_texture) key = "placeholder_icon";

            }

            ImGui::EndChild();

        }

        ImGui::EndTabItem();
    }

    if(ImGui::BeginTabItem("Shaders")){

        if(ImGui::Button("[+] Add Shader")){
            SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenShader, _level_editor_tab, engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), true);
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
                _level_editor_tab->this_level->ResourcesEdited();

                if(shader_key == name_of_erased_shader) shader_key = "partial_sprite_shader";

            }

            ImGui::EndChild();
        }


        ImGui::EndTabItem();

    }
}

void Sprite::OnUpdateEditorViewport([[maybe_unused]] UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    editor_hitbox.size = Vector2f(frame_size.x*scale.x,frame_size.y*scale.y);
    editor_hitbox.position = -offset;
}

void Sprite::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    Actor::OnViewProperties(_level_editor_tab, _index);

    ImGui::InputFloat("offset.x",&offset.x);
    ImGui::InputFloat("offset.y",&offset.y);
    ImGui::InputFloat("frame_size.x",&frame_size.x);
    ImGui::InputFloat("frame_size.y",&frame_size.y);
    ImGui::InputFloat("scale.x",&scale.x);
    ImGui::InputFloat("scale.y",&scale.y);
    ImGui::InputFloat("rotation (degrees)",&rotation);
    ImGui::InputFloat("current_frame_index",&current_frame_index);

    ImVec4 start_colour =  ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    if(ImGui::ColorPicker4(std::string("MyColor##4"+std::to_string(editor_id)).c_str(), (float*)&im_colour, ImGuiColorEditFlags_AlphaBar, (float*)&start_colour)){
        tint = ufo::Colour(im_colour.x*255.0f, im_colour.y*255.0f, im_colour.z*255.0f, im_colour.w*255.0f);
        Console::PrintLine(im_colour.x*255.0f, im_colour.y*255.0f, im_colour.z*255.0f, im_colour.w*255.0f);
    }

    ImGui::Text("Shader: %s", shader_key.c_str());

}

void Sprite::OnAdditionalButtonsForTreeItem(){
    ImGui::SameLine();

    std::string visible_or_not_string = visible ? "<o>###" : "</>###";

    if(ImGui::Button((visible_or_not_string+std::to_string(editor_id)).c_str(), ImVec2(0,ImGui::GetFontSize()))){
        visible = !visible;
    }
}

void Sprite::OnResourcesEdited(){
    if(!engine->asset_manager.textures.count(key)){
        key = "placeholder_icon";
    }
}

#endif //UFO_ENGINE_STUDIO

}
