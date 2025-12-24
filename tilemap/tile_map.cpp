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

void TileMap::InitEditorProperties(){
    //Actor::InitEditorProperties();
    editor_properties.push_back(std::make_unique<EditorPropertyIntHandle>("Number of Columns","Number of Columns",&number_of_columns));
    editor_properties.push_back(std::make_unique<EditorPropertyIntHandle>("Number of Columns","Number of Rows",&number_of_rows));
}

void TileMap::OnSpawn(){
    Actor::OnSpawn();

}

TileMap::TileMap(Vector2f _) : Actor(_){
    base_class_name = "TileMap";
    class_name = base_class_name;
    InitEditorProperties();
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
    int tile_id = tilemap_data[int(_position.y/16.0f) * number_of_columns + (_position.x/16.0f)];
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
    level->tileset_manager.EditorTilesetWidget(_level_editor_tab);
}

void TileMap::OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor){
    if(ImGui::IsItemClicked(0)){
        ImVec2 mouse_pos = ImGui::GetMousePos();
        ImVec2 item_rect_pos = ImGui::GetItemRectMin();

        int clicked_tile_x = int(mouse_pos.x-item_rect_pos.x)/tile_width;
        int clicked_tile_y = int(mouse_pos.y-item_rect_pos.y)/tile_height;

        int tile_to_be_set = clicked_tile_y*number_of_columns + clicked_tile_x;
        tilemap_data[tile_to_be_set] = level->tileset_manager.currently_selected_tile;
    }
}

ufo::gc::JsonMap* TileMap::GetAsJson(ufo::GarbageCollector* _gc){
    Console::PrintLine("Does this even run?");

    ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);
    ufo::gc::JsonArray* tiles = _gc->New<ufo::gc::JsonArray>();

    for(const auto& i : tilemap_data) tiles->array.push_back(_gc->New<ufo::gc::JsonNumber>(i));
    parent_class_as_json->map.emplace("tiles",tiles);
    return parent_class_as_json;
}
