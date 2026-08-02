#include <algorithm>
#include <exception>
#include <stdexcept>
#include <vector>
#include <memory>
#include "tile_map.h"
#include "../src/camera.h"
#include "../ufo_maths/ufo_maths.h"
#include "../src/actor.h"
#include "../src/level.h"
#include "../ufo_garbage_collector/gc_json.h"
#include "../ufo_garbage_collector/garbage_collector.h"
#include "../src/engine.h"
#include "../src/graphics.h"
#include "../src/engine.h"
#include "console.h"
#include "im_vec.h"
#include "tileset_manager.h"
#include "../src/ufo_macros.h"

#ifdef UFO_ENGINE_STUDIO
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"
#include "../src/actor_undo_and_redo.h"
#include "../imgui/misc/cpp/imgui_stdlib.h"
#endif

namespace ufo{

TileMap::TileMap(Vector2f _) : Actor(_){
    base_class_name = "ufo::TileMap";
    class_name = base_class_name;
    tilemap_data = std::vector<int>(number_of_rows*number_of_columns, 0);
}

void TileMap::OnSpawn(){
    Actor::OnSpawn();

}

int TileMap::GetTileID_AtLevelPosition(Vector2f _position){
    int tile_id = tilemap_data[int(_position.y/tile_height) * number_of_columns + (_position.x/tile_width)];
    return tile_id;
}

ufo::Rectangle
TileMap::GetRectangle(int _x, int _y, Vector2f _frame_size){
    ufo::Rectangle rect = ufo::Rectangle({(float)(_x * _frame_size.x), (float)(_y * _frame_size.y)}, _frame_size);
    return rect;
}

ufo::Rectangle
TileMap::GetFrameFromSpriteSheet(int _sprite_width, int _frame, Vector2f _frame_size){
    return GetRectangle(
        (int)_frame % (_sprite_width/(int)_frame_size.x), //1 can only give me x = 0
        (int)_frame / (_sprite_width/(int)_frame_size.x),
        _frame_size); //1 can only give y = 1
}

void TileMap::OnDraw(ufo::Graphics* _graphics, Camera* _camera){
    if(!visible) return;

    float scale = _camera->scale;

    if(!engine->asset_manager.shaders.count(shader_key)){
        Console::PrintLine(GetInfo() + " " + __UFO_PRETTY_FUNCTION__,"Error, shader_key",shader_key,"does not exist");
        return;
    }

    //Haven't made a TilesetManager yet.
    for(auto&& tileset : level->tileset_manager.tileset_data){

        if(!engine->asset_manager.textures.count(tileset.name)){
            Console::PrintLine(GetInfo() + " " + __UFO_PRETTY_FUNCTION__," Error, tileset:",tileset.name,"does not exist");
            continue;
        }

        ufo::Rectangle screen_rectangle = _camera->GetOnScreenRectangleInWorld({tileset.tile_width, tileset.tile_height});

        int tile_start_x = int(std::floor(screen_rectangle.position.x/tileset.tile_width));
        int tile_end_x = int(std::floor((screen_rectangle.position.x + screen_rectangle.size.x)/tileset.tile_width));
        int tile_start_y = int(std::floor(screen_rectangle.position.y/tileset.tile_height));
        int tile_end_y = int(std::floor((screen_rectangle.position.y + screen_rectangle.size.y)/tileset.tile_height));

        tile_start_x = std::max(tile_start_x, 0);
        tile_end_x = std::min(tile_end_x, number_of_columns);
        tile_start_y = std::max(tile_start_y, 0);
        tile_end_y = std::min(tile_end_y, number_of_rows);

        for(int index_y = tile_start_y; index_y < tile_end_y; index_y++){
            for(int index_x = tile_start_x; index_x < tile_end_x; index_x++){
                int tile_id = tilemap_data[index_y*number_of_columns + index_x];

                olc::vd2d tile_position = {index_x*tileset.tile_width, index_y*tileset.tile_height};

                if(tileset.tileset_start_id <= tile_id && tile_id < tileset.tileset_start_id+tileset.tile_count){

                    int sprite_width = 0;
                    try{
                        sprite_width = engine->asset_manager.textures.at(tileset.name).width;

                        ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(sprite_width,tile_id-tileset.tileset_start_id,{tileset.tile_width, tileset.tile_height});
                        //Console::Out("sample rectangle:", sample_rectangle.position, sample_rectangle.size);
                        _graphics->DrawPartialSprite(
                            tileset.name,
                            _camera->Transform(tile_position),
                            {0.0f, 0.0f},
                            {scale, scale},
                            sample_rectangle.position,
                            sample_rectangle.size,
                            0.0f,
                            tint, shader_key, 0.0f
                        );
                    }
                    catch(const std::exception& _error){
                        Console::PrintLine(__UFO_PRETTY_FUNCTION__,"TileMap::OnDraw Error, something went wrong trying to draw tilemap");
                        continue;
                    }
                }

            }
        }

    }

}

ufo::gc::JsonMap* TileMap::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
    ufo::gc::JsonArray* tiles = _gc->New<ufo::gc::JsonArray>();

    for(const auto& i : tilemap_data) tiles->array.push_back(_gc->New<ufo::gc::JsonNumber>(i));
    parent_class_as_json->map.emplace("tiles",tiles);
    parent_class_as_json->map.emplace("number_of_columns", _gc->New<ufo::gc::JsonNumber>(number_of_columns));
    parent_class_as_json->map.emplace("number_of_rows", _gc->New<ufo::gc::JsonNumber>(number_of_rows));
    parent_class_as_json->map.emplace("tile_width", _gc->New<ufo::gc::JsonNumber>(tile_width));
    parent_class_as_json->map.emplace("tile_height", _gc->New<ufo::gc::JsonNumber>(tile_height));
    parent_class_as_json->map.emplace("visible", _gc->New<ufo::gc::JsonNumber>(visible));

    parent_class_as_json->map.emplace("shader_key", _gc->New<ufo::gc::JsonString>(shader_key));

    ufo::gc::JsonArray* j_colour = _gc->New<ufo::gc::JsonArray>();
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.r));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.g));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.b));
    j_colour->array.push_back(_gc->New<ufo::gc::JsonNumber>(tint.a));

    parent_class_as_json->map.emplace("tint", j_colour);

    return parent_class_as_json;
}

void TileMap::OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
    if(_json->map.count("tiles")){
        auto tiles = _json->map.at("tiles")->AsArray();
        tilemap_data.clear();
        tilemap_data.reserve(tiles.size());

        for(const auto& tile : tiles){
            tilemap_data.push_back((int)tile->AsFloat());
        }
    }
    else{
        Console::PrintLine(GetInfo() + " " + __UFO_PRETTY_FUNCTION__, "Error, tiles do not exist");
    }

    number_of_columns = (int)_json->map.at("number_of_columns")->AsFloat();
    number_of_rows = (int)_json->map.at("number_of_rows")->AsFloat();

    _json->TryToGetValueAsInt("tile_width", tile_width, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);
    _json->TryToGetValueAsInt("tile_height", tile_height, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);

    visible = (int)_json->map.at("visible")->AsFloat();

    _json->TryToGetValueAsString("shader_key", shader_key, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);

    std::vector<gc::Json *> j_colour;
    _json->TryToGetValueAsArray("tint", j_colour, GetInfo() + " " + __UFO_PRETTY_FUNCTION__);
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
}

bool TileMap::IsTileWithinBounds(int _x, int _y){

    if(_x >= 0 && _x < number_of_columns && _y >= 0 && _y < number_of_rows) return true;
    return false;

}

#ifdef UFO_ENGINE_STUDIO

Actor* TileMap::OnGetFocusedActor([[maybe_unused]] Vector2f _mouse_position_over_screenspace) {
    return nullptr;
}

void TileMap::OnUtiliseAssetManager(UFOEngineStudio::LevelEditorTab* _level_editor_tab){
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

                bool view_asset_details = ImGui::CollapsingHeader(std::string(("name:"+name)+"###view_asset_details"+name).c_str(), nullptr, ImGuiTreeNodeFlags_SpanTextWidth);

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

                    ImGui::Text(("name:"+name).c_str(),"%s");
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

    if(ImGui::BeginTabItem("Tiles")){

        level->tileset_manager.EditorTilesetWidget(_level_editor_tab);

        ImGui::EndTabItem();

    }
}

void TileMap::DoResize(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _left, int _right, int _bottom, int _top){
    level->RemoveFutureChanges();

    std::unique_ptr<ActorChange_MultipleActorChange> multiple_actor_change = std::make_unique<ActorChange_MultipleActorChange>(false);

    std::unique_ptr<TileMapChange_Paint> paint_action_backup = BackupTilesBeforeResize(_level_editor_tab,_left,_right,_bottom,_top);
    if(paint_action_backup) paint_action_backup->Do(); // Only do backup of tiles with paint action if part of tilemap is removed

    if(_right != 0) ResizeRight(number_of_tiles_to_insert);
    if(_left != 0) ResizeLeft(number_of_tiles_to_insert);
    if(_top != 0) ResizeTop(number_of_tiles_to_insert);
    if(_bottom != 0) ResizeBottom(number_of_tiles_to_insert);

    if(paint_action_backup) multiple_actor_change->changes.push_back(std::move(paint_action_backup)); // Only do backup of tiles with paint action if part of tilemap is removed
    multiple_actor_change->changes.push_back(std::make_unique<TileMap::TileMapChange_TileMapSize>(_level_editor_tab,editor_id,_left,_right,_bottom,_top));

    level->level_changes.push_back(std::move(multiple_actor_change));

    CancelAllResizeDialogues();

}

std::unique_ptr<ufo::TileMap::TileMapChange_Paint> TileMap::BackupTilesBeforeResize(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _left, int _right, int _bottom, int _top){

    //No need to do a backup of tiles if there is no negative resizing done. Tiles aren't removed, so you don't need to back up.
    if(_left > -1 && _right > -1 && _top > -1 && _bottom > -1) return nullptr;

    int x0 = 0;
    int x1 = number_of_columns;
    int y0 = 0;
    int y1 = number_of_rows;

    if(_left < 0){
        x0 = 0;
        x1 = std::abs(_left);
    }
    if(_right < 0){
        x0 = number_of_columns-std::abs(_right);
        x1 = number_of_columns;
    }

    //I'm not very comfortable with how _top is top of the screen (0.0) here. It's inconsistent with my other usages of _top and _bottom.
    if(_top < 0){
        y0 = 0;
        y1 = std::abs(_top);
    }
    if(_bottom < 0){
        y0 = number_of_rows-std::abs(_bottom);
        y1 = number_of_rows;
    }

    tilemap_data_before_change = tilemap_data;

    /*for(int t = 0; t < (x1-x0)*(y1-y0); t++){
        int xx = t%(x1-x0);
        int yy = t/(x1-x0);
        Console::PrintLine(t);

        tilemap_data[(x0+xx)+(y0+yy)*number_of_columns] = 0;

        }*/

    return std::make_unique<TileMapChange_Paint>(
        _level_editor_tab,
        this->editor_id,
        x0,
        y0,
        x1,
        y1
    );
}

void TileMap::ResizeRight(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_columns < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize right");

    if(_number_of_tiles_to_insert > 0){
        int start_index = number_of_columns;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert; j++) tilemap_data.insert(tilemap_data.begin()+start_index, 0);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }
    else{
        int start_index = number_of_columns+_number_of_tiles_to_insert;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert*-1; j++) tilemap_data.erase(tilemap_data.begin()+start_index);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }

    number_of_columns += _number_of_tiles_to_insert;
}

void TileMap::ResizeLeft(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_columns < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize left");

    if(_number_of_tiles_to_insert > 0){
        int start_index = 0;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert; j++) tilemap_data.insert(tilemap_data.begin()+start_index, 0);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }
    else{
        int start_index = 0;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < _number_of_tiles_to_insert*-1; j++) tilemap_data.erase(tilemap_data.begin()+start_index);
            start_index+=(_number_of_tiles_to_insert+number_of_columns);
        }
    }

    number_of_columns += _number_of_tiles_to_insert;
}

void TileMap::ResizeBottom(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_rows < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize bottom");

    if(_number_of_tiles_to_insert > 0){
        int add_at_index = number_of_columns*number_of_rows;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert; i++){
            tilemap_data.insert(tilemap_data.begin()+add_at_index, 0);
        }
    }
    else{
        int remove_at_index = number_of_columns*number_of_rows - number_of_columns*_number_of_tiles_to_insert;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert*-1; i++) tilemap_data.erase(tilemap_data.begin()+remove_at_index);
    }

    number_of_rows += _number_of_tiles_to_insert;
}

void TileMap::ResizeTop(int _number_of_tiles_to_insert){
    if(_number_of_tiles_to_insert+number_of_rows < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize top");

    if(_number_of_tiles_to_insert > 0){
        int add_at_index = 0;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert; i++){
            tilemap_data.insert(tilemap_data.begin()+add_at_index, 0);
        }
    }
    else{
        int remove_at_index = 0;
        for(int i = 0; i < number_of_columns*_number_of_tiles_to_insert*-1; i++) tilemap_data.erase(tilemap_data.begin()+remove_at_index);
    }

    number_of_rows += _number_of_tiles_to_insert;
}

void TileMap::CancelAllResizeDialogues(){
    resize_right = false;
    resize_left = false;
    resize_top = false;
    resize_bottom = false;
}

void TileMap::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    float scale = _camera->scale;

    if(
        (_level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_BRUSH ||
        _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_ERASER ||
        _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_FILL_BUCKET
    )){

        int xx = 0;
        int yy = 0;
        for(const int i : level->tileset_manager.currently_selected_tiles.tiles){

            for(auto&& tileset : level->tileset_manager.tileset_data){

                int tile_id = i;

                olc::vd2d tile_position = {(currently_hovered_tile_x+ xx)*tileset.tile_width, (currently_hovered_tile_y+yy)*tileset.tile_height};

                if(tileset.tileset_start_id <= tile_id && tile_id < tileset.tileset_start_id+tileset.tile_count){
                    int sprite_width = 0;
                    try{
                        sprite_width = engine->asset_manager.textures.at(tileset.name).width;

                        ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(sprite_width,tile_id-tileset.tileset_start_id,{tileset.tile_width, tileset.tile_height});
                        //Console::Out("sample rectangle:", sample_rectangle.position, sample_rectangle.size);
                        _graphics->DrawPartialSprite(
                            tileset.name,
                            _camera->Transform(tile_position),
                            {0.0f, 0.0f},
                            {scale, scale},
                            sample_rectangle.position,
                            sample_rectangle.size,
                            0.0f,
                            tint,
                            shader_key, 0.0f
                        );

                    }
                    catch(const std::exception& _error){
                        Console::PrintLine("TileMap error");
                        continue;
                    }
                }

            }

            //int tile_to_be_set = (currently_hovered_tile_y+yy)*number_of_columns + (currently_hovered_tile_x+xx);

            //if(tile_to_be_set > -1 && tile_to_be_set < tilemap_data.size()) tilemap_data[tile_to_be_set] = i;

            xx++;
            if(xx >= level->tileset_manager.currently_selected_tiles.number_of_columns){
                xx = 0;
                yy++;
            }

        }
    }

    //For copy paste
    if(_level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RECTANGLE_SELECTION){

        int xx = 0;
        int yy = 0;
        for(const int tile_id : level->tileset_manager.tilemap_selected_tiles){

            for(auto&& tileset : level->tileset_manager.tileset_data){

                olc::vd2d tile_position = {(currently_hovered_tile_x+ xx)*tileset.tile_width, (currently_hovered_tile_y+yy)*tileset.tile_height};

                if(tileset.tileset_start_id <= tile_id && tile_id < tileset.tileset_start_id+tileset.tile_count){
                    int sprite_width = 0;
                    try{
                        sprite_width = engine->asset_manager.textures.at(tileset.name).width;

                        ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(sprite_width,tile_id-tileset.tileset_start_id,{tileset.tile_width, tileset.tile_height});
                        //Console::Out("sample rectangle:", sample_rectangle.position, sample_rectangle.size);
                        _graphics->DrawPartialSprite(
                            tileset.name,
                            _camera->Transform(tile_position),
                            {0.0f, 0.0f},
                            {scale, scale},
                            sample_rectangle.position,
                            sample_rectangle.size,
                            0.0f,
                            tint,
                            shader_key, 0.0f
                        );

                    }
                    catch(const std::exception& _error){
                        Console::PrintLine("TileMap error");
                        continue;
                    }
                }

            }

            xx++;
            if(xx >= (int)level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.size.x){
                xx = 0;
                yy++;
            }

        }
    }
}

void TileMap::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){

    if(ImGui::CollapsingHeader("Custom Colouring###TileMap::OnViewProperties_show_colour_picker")){
        ImVec4 start_colour =  ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        if(ImGui::ColorPicker4(std::string("MyColor##4"+std::to_string(editor_id)).c_str(), (float*)&im_colour, ImGuiColorEditFlags_AlphaBar, (float*)&start_colour)){
            tint = ufo::Colour(im_colour.x*255.0f, im_colour.y*255.0f, im_colour.z*255.0f, im_colour.w*255.0f);
            Console::PrintLine(im_colour.x*255.0f, im_colour.y*255.0f, im_colour.z*255.0f, im_colour.w*255.0f);
        }

        ImGui::Text("Shader: %s", shader_key.c_str());
    }

    ImGui::Separator();

    ImGui::Checkbox(std::string("Visible###"+std::to_string(editor_id)).c_str(), &visible);

    ImGui::InputInt("Tile Width", &tile_width);
    ImGui::InputInt("Tile Height", &tile_height);

    ImGui::Text("Tilemap Width: %i tiles", number_of_columns);
    ImGui::Text("Tilemap Height: %i tiles", number_of_rows);

    //Currently hovered tiles

    ImGui::Text("Tile x: %i tiles", currently_hovered_tile_x);
    ImGui::Text("Tile y: %i tiles", currently_hovered_tile_y);

    ImGui::Text("Current world mouse x: %f", world_mouse.x);
    ImGui::Text("Current world mouse y: %f", world_mouse.y);

    try{

    if(!resize_right){
        if(ImGui::Button("Resize Right")){
            CancelAllResizeDialogues();
            resize_right = true;
            _level_editor_tab->current_undo_redo_action.tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RESIZE;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){

            DoResize(_level_editor_tab,0,number_of_tiles_to_insert,0,0);

        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_right = false;
        }
    }

    if(!resize_left){
        if(ImGui::Button("Resize Left")){
            CancelAllResizeDialogues();
            resize_left = true;
            _level_editor_tab->current_undo_redo_action.tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RESIZE;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){

            DoResize(_level_editor_tab,number_of_tiles_to_insert,0,0,0);

        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_left = false;
        }
    }

    if(!resize_bottom){
        if(ImGui::Button("Resize Bottom")){
            CancelAllResizeDialogues();
            resize_bottom = true;
            _level_editor_tab->current_undo_redo_action.tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RESIZE;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){

            DoResize(_level_editor_tab,0,0,number_of_tiles_to_insert,0);

        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_bottom = false;
        }
    }

    if(!resize_top){
        if(ImGui::Button("Resize Top")){
            CancelAllResizeDialogues();
            resize_top = true;
            _level_editor_tab->current_undo_redo_action.tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RESIZE;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){

            DoResize(_level_editor_tab,0,0,0,number_of_tiles_to_insert);

        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_top = false;
        }
    }

    } catch(const std::runtime_error& _error){
        Console::PrintLine("[UFO-Engine] TileMap::OnViewProperties", _error.what());
        CancelAllResizeDialogues();
    }

    ImGui::Separator();

    ImGui::Checkbox("Enable Autotiling", &is_autotiling_enabled);
    if(is_autotiling_enabled){
        if(autotiling_file == ""){
            if(ImGui::Button("Add autotile file")){
                SDL_ShowOpenFileDialog(&UFOEngineStudio::OnOpenAutoTileFile, this, _level_editor_tab->engine->window, nullptr, 0, _level_editor_tab->editor->opened_directory_path.c_str(), false);
            }
        }
        else{
            ImGui::Text("%s", autotiling_file.c_str()); ImGui::SameLine();
            if(ImGui::Button("Clear###Clear Autotile File")){
                autotiling_file = "";
                auto_tiling_tilemap_owner = nullptr;
                auto_tiling_tilemap = nullptr;
            }
        }
    }

    //ImGui::Separator(); level->tileset_manager.EditorTilesetWidget(_level_editor_tab);
}

bool TileMap::OnEndUndoRedoAction(UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    if(_level_editor_tab->current_undo_redo_action.tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_SELECTION_DELETE){

        int x0 = level->tileset_manager.tilemap_rectangular_selection.position.x;
        int x1 = level->tileset_manager.tilemap_rectangular_selection.position.x+level->tileset_manager.tilemap_rectangular_selection.size.x;
        int y0 = level->tileset_manager.tilemap_rectangular_selection.position.y;
        int y1 = level->tileset_manager.tilemap_rectangular_selection.position.y+level->tileset_manager.tilemap_rectangular_selection.size.y;

        tilemap_data_before_change = tilemap_data;

        for(int t = 0; t < (x1-x0)*(y1-y0); t++){
            int xx = t%(int)(x1-x0);
            int yy = t/(int)(x1-x0);
            Console::PrintLine(t);

            if(IsTileWithinBounds(x0+xx, y0+yy)) tilemap_data[(x0+xx)+(y0+yy)*number_of_columns] = 0;

        }

        _level_editor_tab->current_undo_redo_action.actor_change = std::make_unique<TileMapChange_Paint>(
            _level_editor_tab,
            this->editor_id,
            x0,
            y0,
            x1,
            y1
        );

        return true;

    }

    if(_level_editor_tab->current_undo_redo_action.tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_PASTE){

        int x0 = level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.position.x;
        int x1 = level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.position.x+level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.size.x;
        int y0 = level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.position.y;
        int y1 = level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.position.y+level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.size.y;

        tilemap_data_before_change = tilemap_data;

        for(int t = 0; t < (int)level->tileset_manager.tilemap_selected_tiles.size(); t++){
            int xx = t%(int)(x1-x0);
            int yy = t/(int)(x1-x0);

            if(IsTileWithinBounds(currently_hovered_tile_x+xx, currently_hovered_tile_y+yy)) tilemap_data[currently_hovered_tile_x+xx+(currently_hovered_tile_y+yy)*number_of_columns] = level->tileset_manager.tilemap_selected_tiles[t];

        }

        _level_editor_tab->current_undo_redo_action.actor_change = std::make_unique<TileMapChange_Paint>(
            _level_editor_tab,
            this->editor_id,
            currently_hovered_tile_x,
            currently_hovered_tile_y,
            currently_hovered_tile_x+(x1-x0),
            currently_hovered_tile_y+(y1-y0));

        level->tileset_manager.tilemap_rectangular_selection.position = Vector2f(currently_hovered_tile_x, currently_hovered_tile_y);
        level->tileset_manager.tilemap_rectangular_selection.size = level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles.size;

        return true;

    }

    if(ImGui::IsMouseReleased(ImGuiMouseButton_Left)){

        if(_level_editor_tab->current_undo_redo_action.tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_BRUSH){
            _level_editor_tab->current_undo_redo_action.actor_change = std::make_unique<TileMapChange_Paint>(
                _level_editor_tab,
                this->editor_id,
                left_bound_tile,
                lower_bound_tile,
                right_bound_tile,
                upper_bound_tile
            );

            if(is_autotiling_enabled && autotiling_file != ""){
                int tilemap_top = (currently_hovered_tile_x+(currently_hovered_tile_y-1)*number_of_columns);
                int tilemap_top_right = (currently_hovered_tile_x+1)+(currently_hovered_tile_y-1)*number_of_columns;
                int tilemap_top_left = (currently_hovered_tile_x-1)+(currently_hovered_tile_y-1)*number_of_columns;

                int tilemap_bottom = (currently_hovered_tile_x+(currently_hovered_tile_y+1)*number_of_columns);
                int tilemap_bottom_right = (currently_hovered_tile_x+1)+(currently_hovered_tile_y+1)*number_of_columns;
                int tilemap_bottom_left = (currently_hovered_tile_x-1)+(currently_hovered_tile_y+1)*number_of_columns;

                int tilemap_right = ((currently_hovered_tile_x+1)+currently_hovered_tile_y*number_of_columns);
                int tilemap_left = ((currently_hovered_tile_x-1)+currently_hovered_tile_y*number_of_columns);
                int tilemap_centre = currently_hovered_tile_x+currently_hovered_tile_y*number_of_columns;

                for(int t = 2+auto_tiling_tilemap->number_of_columns*2; t < (int)auto_tiling_tilemap->tilemap_data.size(); t+=4){

                    int xx = t%(int)(auto_tiling_tilemap->number_of_columns);
                    int yy = t/(int)(auto_tiling_tilemap->number_of_columns);

                    if(!IsTileWithinBounds(xx, yy)) continue;

                    if(xx > auto_tiling_tilemap->number_of_columns){
                        t+=auto_tiling_tilemap->number_of_columns*4;
                        t-= 4;
                    }

                    Console::PrintLine("autotiling: analysing tiles",auto_tiling_tilemap->tilemap_data[t]);

                    int top = xx+(yy-1)*auto_tiling_tilemap->number_of_columns;
                    int top_right = (xx+1)+(yy-1)*auto_tiling_tilemap->number_of_columns;
                    int top_left = (xx-1)+(yy-1)*auto_tiling_tilemap->number_of_columns;
                    int bottom = xx+(yy+1)*auto_tiling_tilemap->number_of_columns;
                    int bottom_right = (xx+1)+(yy+1)*auto_tiling_tilemap->number_of_columns;
                    int bottom_left = (xx-1)+(yy+1)*auto_tiling_tilemap->number_of_columns;
                    int right = (xx+1)+yy*auto_tiling_tilemap->number_of_columns;
                    int left = (xx-1)+yy*auto_tiling_tilemap->number_of_columns;
                    int centre = t;

                    if(ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[top]) == ufo::Maths::SignInt(tilemap_data[tilemap_top]) &&
                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[bottom]) == ufo::Maths::SignInt(tilemap_data[tilemap_bottom]) &&
                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[right]) == ufo::Maths::SignInt(tilemap_data[tilemap_right]) &&
                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[left]) == ufo::Maths::SignInt(tilemap_data[tilemap_left]) &&

                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[top_left]) == ufo::Maths::SignInt(tilemap_data[tilemap_top_left]) &&
                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[top_right]) == ufo::Maths::SignInt(tilemap_data[tilemap_top_right]) &&
                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[bottom_left]) == ufo::Maths::SignInt(tilemap_data[tilemap_bottom_left]) &&
                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[bottom_right]) == ufo::Maths::SignInt(tilemap_data[tilemap_bottom_right]) &&

                        ufo::Maths::SignInt(auto_tiling_tilemap->tilemap_data[centre]) == ufo::Maths::SignInt(tilemap_data[tilemap_centre])
                    ){
                        tilemap_data[tilemap_top] = auto_tiling_tilemap->tilemap_data[top];
                        tilemap_data[tilemap_top_right] = auto_tiling_tilemap->tilemap_data[top_right];
                        tilemap_data[tilemap_top_left] = auto_tiling_tilemap->tilemap_data[top_left];
                        tilemap_data[tilemap_bottom] = auto_tiling_tilemap->tilemap_data[bottom];
                        tilemap_data[tilemap_bottom_right] = auto_tiling_tilemap->tilemap_data[bottom_right];
                        tilemap_data[tilemap_bottom_left] = auto_tiling_tilemap->tilemap_data[bottom_left];
                        tilemap_data[tilemap_right] = auto_tiling_tilemap->tilemap_data[right];
                        tilemap_data[tilemap_left] = auto_tiling_tilemap->tilemap_data[left];
                        tilemap_data[tilemap_centre] = auto_tiling_tilemap->tilemap_data[centre];
                    }

                }
            }
        }
        else if(_level_editor_tab->current_undo_redo_action.tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_FILL_BUCKET){
            bool tiles_are_being_added = true;

            std::vector<Vector2i> all_filled_tiles; //For undo&redo

            std::vector<Vector2i> tiles_to_fill;

            if(!IsTileWithinBounds(currently_hovered_tile_x, currently_hovered_tile_y)) return false;

            int tile_to_replace = tilemap_data[currently_hovered_tile_y*number_of_columns+currently_hovered_tile_x];

            if(tile_to_replace == level->tileset_manager.currently_selected_tiles.first_selected_tile) return false;

            tiles_to_fill.push_back(Vector2i(currently_hovered_tile_x, currently_hovered_tile_y));

            int max_number_of_tiles = 100;

            while(tiles_are_being_added && max_number_of_tiles > 0){
                max_number_of_tiles--;

                std::vector<Vector2i> additional_tiles;

                tiles_are_being_added = false;
                for(Vector2i tile_position : tiles_to_fill){

                    std::vector<Vector2i> directions = {
                        tile_position+Vector2i(1, 0),
                        tile_position+Vector2i(-1, 0),
                        tile_position+Vector2i(0, -1),
                        tile_position+Vector2i(0, 1)

                    };

                    for(Vector2i tile_direction : directions){
                        if(!IsTileWithinBounds(tile_direction.x, tile_direction.y)) continue;

                        int tile_index = tile_direction.y*number_of_columns + tile_direction.x;

                        if(tilemap_data[tile_index] == tile_to_replace){

                            tilemap_data[tile_index] = level->tileset_manager.currently_selected_tiles.first_selected_tile;

                            additional_tiles.push_back(tile_direction);
                            all_filled_tiles.push_back(tile_direction);

                            tiles_are_being_added = true;

                        }

                    }
                }

                tiles_to_fill = additional_tiles;

            }

            if(max_number_of_tiles <= 0) Console::PrintLine("TileMap: Reached max tiles");

            int lower_bound_tile_fill = currently_hovered_tile_y;
            int upper_bound_tile_fill = currently_hovered_tile_y;
            int left_bound_tile_fill = currently_hovered_tile_x;
            int right_bound_tile_fill = currently_hovered_tile_x;

            for(Vector2i tile : all_filled_tiles){
                Console::PrintLine(tile);
                if(lower_bound_tile_fill > tile.y) lower_bound_tile_fill = tile.y;
                if(upper_bound_tile_fill < tile.y) upper_bound_tile_fill = tile.y;
                if(left_bound_tile_fill > tile.x) left_bound_tile_fill = tile.x;
                if(right_bound_tile_fill < tile.x) right_bound_tile_fill = tile.x;
            }

            _level_editor_tab->current_undo_redo_action.actor_change = std::make_unique<TileMapChange_Paint>(
                _level_editor_tab,
                this->editor_id,
                left_bound_tile_fill,
                lower_bound_tile_fill,
                right_bound_tile_fill+1,
                upper_bound_tile_fill+1);

        }
        return true;
    }

    return false;
}

void TileMap::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){

    /*if(_level_editor_tab->actor_dedicated_to_viewport != ufo::Maths::NULL_ID){
        if(level->actors_with_stable_id.at(_level_editor_tab->actor_dedicated_to_viewport)->parent){
            if(level->actors_with_stable_id.at(_level_editor_tab->actor_dedicated_to_viewport)->parent->GetTileMap() != this) return;

        }
    }*/
    if(_level_editor_tab->actor_dedicated_to_viewport_id != editor_id && _level_editor_tab->inspected_actor_id != editor_id) return;

    world_mouse = level->active_camera_handles.back()->TransformScreenToWorld(_level_editor_tab->mouse_position_over_screenspace);

    currently_hovered_tile_x = int(world_mouse.x)/tile_width;
    currently_hovered_tile_y = int(world_mouse.y)/tile_height;

    int currently_hovered_tile_left_boundary = 0-level->tileset_manager.currently_selected_tiles.number_of_columns+1;
    int currently_hovered_tile_lower_boundary = 0-level->tileset_manager.currently_selected_tiles.number_of_rows+1;

    if(currently_hovered_tile_x < currently_hovered_tile_left_boundary) currently_hovered_tile_x = currently_hovered_tile_left_boundary;
    if(currently_hovered_tile_x > number_of_columns-1) currently_hovered_tile_x = number_of_columns-1;
    if(currently_hovered_tile_y < currently_hovered_tile_lower_boundary) currently_hovered_tile_y = currently_hovered_tile_lower_boundary;
    if(currently_hovered_tile_y > number_of_rows-1) currently_hovered_tile_y = number_of_rows-1;

    if(is_selected &&
        (_level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_BRUSH ||
        _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_ERASER ||
        _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_FILL_BUCKET)
    ){

        if(ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(0)){
            tilemap_data_before_change = tilemap_data;
            left_bound_tile = currently_hovered_tile_x;
            lower_bound_tile = currently_hovered_tile_y;
            upper_bound_tile = lower_bound_tile+level->tileset_manager.currently_selected_tiles.number_of_rows;
            right_bound_tile = left_bound_tile+level->tileset_manager.currently_selected_tiles.number_of_columns;

            //If normal tool
            _level_editor_tab->current_undo_redo_action = UFOEngineStudio::LevelEditorTab::UndoRedoAction{
                editor_id, _level_editor_tab->current_tool, nullptr
            };

        }

        //Here the change which ends the placing action was before

        if(_level_editor_tab->current_undo_redo_action.actor_id == editor_id
            && _level_editor_tab->current_undo_redo_action.tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_BRUSH  && ImGui::IsMouseDown(0)
        ){
            if(currently_hovered_tile_x < left_bound_tile) left_bound_tile = currently_hovered_tile_x;
            if(currently_hovered_tile_y < lower_bound_tile) lower_bound_tile = currently_hovered_tile_y;
            if((int)currently_hovered_tile_y +level->tileset_manager.currently_selected_tiles.number_of_rows    > upper_bound_tile) upper_bound_tile = currently_hovered_tile_y+level->tileset_manager.currently_selected_tiles.number_of_rows;
            if((int)currently_hovered_tile_x +level->tileset_manager.currently_selected_tiles.number_of_columns > right_bound_tile) right_bound_tile = currently_hovered_tile_x+level->tileset_manager.currently_selected_tiles.number_of_columns;


            int xx = 0;
            int yy = 0;
            for(const int i : level->tileset_manager.currently_selected_tiles.tiles){

                int tile_to_be_set = (currently_hovered_tile_y+yy)*number_of_columns + (currently_hovered_tile_x+xx);

                if(IsTileWithinBounds(currently_hovered_tile_x+xx, currently_hovered_tile_y+yy)) tilemap_data[tile_to_be_set] = i;

                xx++;
                if(xx >= level->tileset_manager.currently_selected_tiles.number_of_columns){
                    xx = 0;
                    yy++;
                }

            }


        }

    }

    if(is_selected && _level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RECTANGLE_SELECTION){
        if(ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(0)){
            level->tileset_manager.tilemap_rectangular_selection.size = Vector2f(0.0f, 0.0f);
            level->tileset_manager.tilemap_rectangular_selection.position = Vector2f(currently_hovered_tile_x, currently_hovered_tile_y);
            //level->tileset_manager.tilemap_selected_tiles.clear();
            _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RECTANGLE_SELECTION_RESIZE;
        }
    }

    {
        Vector2f bounds_min = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition());
        Vector2f bounds_max = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(number_of_columns*tile_width, number_of_rows*tile_height));

        ImU32 colour = 0x66777755;

        for(int rr = 0; rr < number_of_rows; rr++){
            Vector2f line_start_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(0.0f, rr*tile_height));
            Vector2f line_end_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(number_of_columns*tile_width, rr*tile_height));

            ImGui::GetWindowDrawList()->AddLine(ImVec2(line_start_screen_space.x, line_start_screen_space.y), ImVec2(line_end_screen_space.x, line_end_screen_space.y), colour);
        }

        for(int cc = 0; cc < number_of_columns; cc++){
            Vector2f line_start_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(cc*tile_width, 0.0f));
            Vector2f line_end_screen_space = _level_editor_tab->TranslateToEditorScreenSpace(GetGlobalPosition()+Vector2f(cc*tile_width, number_of_rows*tile_height));

            ImGui::GetWindowDrawList()->AddLine(ImVec2(line_start_screen_space.x, line_start_screen_space.y), ImVec2(line_end_screen_space.x, line_end_screen_space.y), colour);
        }

        ImGui::GetWindowDrawList()->AddRect(ImVec2(bounds_min.x,bounds_min.y), ImVec2(bounds_max.x,bounds_max.y), colour, 1.0f,ImDrawFlags_RoundCornersAll);
    }

    //Draw selection
    if(_level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RECTANGLE_SELECTION_RESIZE){

        level->tileset_manager.tilemap_rectangular_selection.size = Vector2f(currently_hovered_tile_x+1, currently_hovered_tile_y+1)-level->tileset_manager.tilemap_rectangular_selection.position;

        int x0 = level->tileset_manager.tilemap_rectangular_selection.position.x;
        int x1 = level->tileset_manager.tilemap_rectangular_selection.position.x+level->tileset_manager.tilemap_rectangular_selection.size.x;
        int y0 = level->tileset_manager.tilemap_rectangular_selection.position.y;
        int y1 = level->tileset_manager.tilemap_rectangular_selection.position.y+level->tileset_manager.tilemap_rectangular_selection.size.y;

        if(x0 >= x1){
            std::swap(x0,x1);
            x1+=1;
            x0-=1;

        }
        if(y0 >= y1){
            std::swap(y0,y1);
            y1+=1;
            y0-=1;

        }

        if(ImGui::IsItemHovered() && ImGui::IsMouseReleased(0)){

            //Suggestion: re-enter TILE_MAP_BRUSH mode and transfer tilemap_selected_tiles to currently_selected_tiles, allowing the user to paint them.

            level->tileset_manager.tilemap_rectangular_selection = ufo::Rectangle(Vector2f(x0, y0),Vector2f(x1-x0, y1-y0));
            _level_editor_tab->current_tool = UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RECTANGLE_SELECTION;
        }

        ImU32 colour = 0xFFFFFFFF;

        ImVec2 selection_min = UFOEngineStudio::FromVector2fToImVec2(_level_editor_tab->TranslateToEditorScreenSpace(Vector2f(x0,y0)*Vector2f(tile_width, tile_height)));
        ImVec2 selection_max = UFOEngineStudio::FromVector2fToImVec2(_level_editor_tab->TranslateToEditorScreenSpace(Vector2f(x1,y1)*Vector2f(tile_width, tile_height)));

        ImGui::GetWindowDrawList()->AddRect(selection_min, selection_max, colour, 1.0f,ImDrawFlags_RoundCornersAll);
    }
    if(_level_editor_tab->current_tool == UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_RECTANGLE_SELECTION){
        int x0 = level->tileset_manager.tilemap_rectangular_selection.position.x;
        int x1 = level->tileset_manager.tilemap_rectangular_selection.position.x+level->tileset_manager.tilemap_rectangular_selection.size.x;
        int y0 = level->tileset_manager.tilemap_rectangular_selection.position.y;
        int y1 = level->tileset_manager.tilemap_rectangular_selection.position.y+level->tileset_manager.tilemap_rectangular_selection.size.y;

        if(ImGui::IsKeyPressed(ImGuiKey_C) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl)){
            level->tileset_manager.tilemap_rectangular_selection_for_copied_tiles = level->tileset_manager.tilemap_rectangular_selection;
            level->tileset_manager.tilemap_selected_tiles.clear();
            for(int yy = y0; yy < y1; yy++){
                for(int xx = x0; xx < x1; xx++){
                    int tile = tilemap_data[yy*number_of_columns+xx];
                    level->tileset_manager.tilemap_selected_tiles.push_back(tile);

                }
            }
        }

        if(ImGui::IsKeyPressed(ImGuiKey_Delete)){
            _level_editor_tab->current_undo_redo_action = UFOEngineStudio::LevelEditorTab::UndoRedoAction{
                this->editor_id, UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_SELECTION_DELETE, nullptr
            };
        }

        if(ImGui::IsKeyPressed(ImGuiKey_V) && ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsItemHovered()){
            _level_editor_tab->current_undo_redo_action = UFOEngineStudio::LevelEditorTab::UndoRedoAction{
                this->editor_id, UFOEngineStudio::LevelEditorTab::Tools::TILE_MAP_PASTE, nullptr
            };
        }

        ImU32 colour = 0xFFFFFFFF;

        ImVec2 selection_min = UFOEngineStudio::FromVector2fToImVec2(_level_editor_tab->TranslateToEditorScreenSpace(Vector2f(x0,y0)*Vector2f(tile_width, tile_height)));
        ImVec2 selection_max = UFOEngineStudio::FromVector2fToImVec2(_level_editor_tab->TranslateToEditorScreenSpace(Vector2f(x1,y1)*Vector2f(tile_width, tile_height)));

        ImGui::GetWindowDrawList()->AddRect(selection_min, selection_max, colour, 1.0f,ImDrawFlags_RoundCornersAll);
    }

}

void TileMap::OnAdditionalButtonsForTreeItem(){
    ImGui::SameLine();
    std::string visible_or_not_string = visible ? "<o>###" : "</>###";
    if(ImGui::Button(
        (visible_or_not_string+std::to_string(editor_id)).c_str(), ImVec2(0,ImGui::GetFontSize()))
    ){
        visible = !visible;
    }
}

void TileMap::OnResourcesEdited(){
    /*if(!engine->asset_manager.textures.count(key)){
        key = "placeholder_icon";
    }*/
    if(!engine->asset_manager.shaders.count(shader_key)){
        shader_key = "partial_sprite_shader";
    }
}

#endif //UFO_ENGINE_STUDIO

}
