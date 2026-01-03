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

void TileMap::OnSpawn(){
    Actor::OnSpawn();

}

TileMap::TileMap(Vector2f _) : Actor(_){
    base_class_name = "TileMap";
    class_name = base_class_name;
    tilemap_data = std::vector<int>(number_of_rows*number_of_columns, 0);
}

std::unique_ptr<TileMap>
TileMap::Load(ufo::gc::JsonMap* _layer){
    std::unique_ptr<TileMap> u_tilemap = std::make_unique<TileMap>(Vector2f(0.0f,0.0f));

    if(_layer->map.at("type")->AsString() == "tilelayer"){
        std::vector<int> data;
        auto layer_data = _layer->map.at("data")->AsArray();

        for(const auto& _json : layer_data){
            int id = (float)_json->AsFloat();
            u_tilemap->tilemap_data.push_back(id);
        }

    }

    return std::move(u_tilemap);
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
TileMap::GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size){
    return GetRectangle(
        (int)_frame % (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x), //1 can only give me x = 0
        (int)_frame / (engine->asset_manager.textures.at(_sprite_key).width/(int)_frame_size.x),
        _frame_size); //1 can only give y = 1
}

void TileMap::ResizeRight(){
    if(number_of_tiles_to_insert+number_of_columns < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize right");

    if(number_of_tiles_to_insert > 0){
        int start_index = number_of_columns;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < number_of_tiles_to_insert; j++) tilemap_data.insert(tilemap_data.begin()+start_index, 0);
            start_index+=(number_of_tiles_to_insert+number_of_columns);
        }
    }
    else{
        int start_index = number_of_columns+number_of_tiles_to_insert;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < number_of_tiles_to_insert*-1; j++) tilemap_data.erase(tilemap_data.begin()+start_index);
            start_index+=(number_of_tiles_to_insert+number_of_columns);
        }
    }

    number_of_columns += number_of_tiles_to_insert;
}

void TileMap::ResizeLeft(){
    if(number_of_tiles_to_insert+number_of_columns < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize left");

    if(number_of_tiles_to_insert > 0){
        int start_index = 0;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < number_of_tiles_to_insert; j++) tilemap_data.insert(tilemap_data.begin()+start_index, 0);
            start_index+=(number_of_tiles_to_insert+number_of_columns);
        }
    }
    else{
        int start_index = number_of_tiles_to_insert;
        for(int i = 0; i < number_of_rows; i++){
            for(int j = 0; j < number_of_tiles_to_insert*-1; j++) tilemap_data.erase(tilemap_data.begin()+start_index);
            start_index+=(number_of_tiles_to_insert+number_of_columns);
        }
    }

    number_of_columns += number_of_tiles_to_insert;
}

void TileMap::ResizeBottom(){
    if(number_of_tiles_to_insert+number_of_rows < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize bottom");

    if(number_of_tiles_to_insert > 0){
        int add_at_index = number_of_columns*number_of_rows;
        for(int i = 0; i < number_of_columns*number_of_tiles_to_insert; i++){
            tilemap_data.insert(tilemap_data.begin()+add_at_index, 0);
        }
    }
    else{
        int remove_at_index = number_of_columns*number_of_rows - number_of_columns*number_of_tiles_to_insert;
        for(int i = 0; i < number_of_columns*number_of_tiles_to_insert*-1; i++) tilemap_data.erase(tilemap_data.begin()+remove_at_index);
    }

    number_of_rows += number_of_tiles_to_insert;
}

void TileMap::ResizeTop(){
    if(number_of_tiles_to_insert+number_of_rows < 1) throw std::runtime_error("[UFO-Engine] TileMap: Could not resize top");

    if(number_of_tiles_to_insert > 0){
        int add_at_index = 0;
        for(int i = 0; i < number_of_columns*number_of_tiles_to_insert; i++){
            tilemap_data.insert(tilemap_data.begin()+add_at_index, 0);
        }
    }
    else{
        int remove_at_index = 0;
        for(int i = 0; i < number_of_columns*number_of_tiles_to_insert*-1; i++) tilemap_data.erase(tilemap_data.begin()+remove_at_index);
    }

    number_of_rows += number_of_tiles_to_insert;
}

void TileMap::CancelAllResizeDialogues(){
    resize_right = false;
    resize_left = false;
    resize_top = false;
    resize_bottom = false;
}

void TileMap::OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera){

}

void TileMap::OnDraw(ufo::Graphics* _graphics, Camera* _camera){
    float scale = _camera->scale;
    Bounds world_bounds = _camera->world;

    //Haven't made a TilesetManager yet.
    for(auto&& tileset : level->tileset_manager.tileset_data){
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
                    ufo::Rectangle sample_rectangle = GetFrameFromSpriteSheet(tileset.name,tile_id-tileset.tileset_start_id,{tileset.tile_width, tileset.tile_height});
                    //Console::Out("sample rectangle:", sample_rectangle.position, sample_rectangle.size);
                    _graphics->DrawPartialSprite(
                        tileset.name,
                        _camera->Transform(tile_position),
                        {0.0f, 0.0f},
                        {scale, scale},
                        sample_rectangle.position,
                        sample_rectangle.size,
                        0.0f,
                        ufo::Colour(255,255,255,255)
                    );
                }

            }
        }

    }

}

void TileMap::OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index){
    Actor::OnViewProperties(_level_editor_tab, _index);

    ImGui::Separator();

    ImGui::Text("Tilemap Width: %i tiles", number_of_columns);
    ImGui::Text("Tilemap Height: %i tiles", number_of_rows);

    //Currently hovered tiles

    ImGui::Text("Tile x: %i tiles", currently_hovered_tile_x);
    ImGui::Text("Tile y: %i tiles", currently_hovered_tile_y);

    try{

    if(!resize_right){
        if(ImGui::Button("Resize Right")){
            resize_right = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeRight();

            resize_right = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_right = false;
        }
    }

    if(!resize_left){
        if(ImGui::Button("Resize Left")){
            resize_left = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeLeft();

            resize_left = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_left = false;
        }
    }

    if(!resize_bottom){
        if(ImGui::Button("Resize Bottom")){
            resize_bottom = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeBottom();

            resize_bottom = false;
        }
        ImGui::SameLine();
        if(ImGui::Button("Cancel")){
            resize_bottom = false;
        }
    }

    if(!resize_top){
        if(ImGui::Button("Resize Top")){
            resize_top = true;
        }
    }
    else{
        ImGui::InputInt("Number of tiles:", &number_of_tiles_to_insert);
        if(ImGui::Button("Ok")){
            ResizeTop();

            resize_top = false;
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

    level->tileset_manager.EditorTilesetWidget(_level_editor_tab);
}

void TileMap::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab){
    if(!properties_open) return;

    ImVec2 mouse_pos = ImGui::GetMousePos();
    ImVec2 item_rect_pos = ImGui::GetItemRectMin();

    int hovered_tile_x = int(mouse_pos.x-item_rect_pos.x)/tile_width;
    int hovered_tile_y = int(mouse_pos.y-item_rect_pos.y)/tile_height;

    if(hovered_tile_x < 0) hovered_tile_x = 0;
    if(hovered_tile_x > number_of_columns-1) hovered_tile_x = number_of_columns-1;
    if(hovered_tile_y < 0) hovered_tile_y = 0;
    if(hovered_tile_y > number_of_rows-1) hovered_tile_y = number_of_rows-1;

    currently_hovered_tile_x = hovered_tile_x;
    currently_hovered_tile_y = hovered_tile_y;

    if(ImGui::IsItemHovered(0) && ImGui::IsMouseClicked(0)){

        int xx = 0;
        int yy = 0;
        for(const int i : level->tileset_manager.currently_selected_tiles.tiles){

            int tile_to_be_set = (hovered_tile_y+yy)*number_of_columns + (hovered_tile_x+xx);

            if(tile_to_be_set > -1 && tile_to_be_set < tilemap_data.size()) tilemap_data[tile_to_be_set] = i;

            Console::PrintLine("Placed in TileMap",i,hovered_tile_x+xx, hovered_tile_y+yy);

            xx++;
            if(xx >= level->tileset_manager.currently_selected_tiles.number_of_columns){
                xx = 0;
                yy++;
            }

        }

    }

    /*if(ImGui::IsItemHovered(0) && ImGui::IsMouseDown(0)){

        int tile_to_be_set = hovered_tile_y*number_of_columns + hovered_tile_x;

        if(tile_to_be_set > -1 && tile_to_be_set < tilemap_data.size()) tilemap_data[tile_to_be_set] = level->tileset_manager.currently_selected_tile;

        }*/
}

ufo::gc::JsonMap* TileMap::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
    ufo::gc::JsonArray* tiles = _gc->New<ufo::gc::JsonArray>();

    for(const auto& i : tilemap_data) tiles->array.push_back(_gc->New<ufo::gc::JsonNumber>(i));
    parent_class_as_json->map.emplace("tiles",tiles);
    parent_class_as_json->map.emplace("number_of_columns", _gc->New<ufo::gc::JsonNumber>(number_of_columns));
    parent_class_as_json->map.emplace("number_of_rows", _gc->New<ufo::gc::JsonNumber>(number_of_rows));
    return parent_class_as_json;
}
