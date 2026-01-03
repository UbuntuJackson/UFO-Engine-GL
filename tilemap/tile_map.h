#pragma once

#include <memory>
#include "../ufo_maths/ufo_maths.h"
#include "../src/actor.h"
#include "../json/json_variant.h"

class Camera;
class Level;

namespace ufo{
    class Graphics;
    namespace gc{
        class JsonMap;
    }
}

class TileMap : public Actor{
public:

    std::vector<int> tilemap_data;
    int number_of_columns = 50;
    int number_of_rows = 10;
    int tile_width = 16;
    int tile_height = 16;

    bool resize_right = false;
    bool resize_left = false;
    bool resize_bottom = false;
    bool resize_top = false;
    int number_of_tiles_to_insert = 0;

    int currently_hovered_tile_x = 0;
    int currently_hovered_tile_y = 0;
    int current_world_mouse_x = 0;
    int current_world_mouse_y = 0;

    TileMap(Vector2f _);

    void OnSpawn();

    static std::unique_ptr<TileMap> Load(ufo::gc::JsonMap* _layer);

    int GetTileID_AtLevelPosition(Vector2f _position);

    ufo::Rectangle GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size);

    void ResizeRight();
    void ResizeLeft();
    void ResizeTop();
    void ResizeBottom();
    void CancelAllResizeDialogues();

    struct TileData{
        int x;
        int y;
        bool within_bounds;
        int tile_number;
        int tile_identifier;
    };

    TileData GetTileID_AtLevelPosition_Advanced(int _x, int _y){
        bool within_bounds = false;
        bool tile_identifier = false;

        int clicked_tile_x = _x/tile_width;
        int clicked_tile_y = _y/tile_height;

        int tile_number = clicked_tile_y*number_of_columns + clicked_tile_x;
        if(clicked_tile_x < number_of_columns && clicked_tile_x >= 0 && clicked_tile_y < number_of_rows && clicked_tile_y >= 0){
            tile_identifier = tilemap_data[tile_number];
            within_bounds = true;
        }

        return TileData{
            clicked_tile_x, clicked_tile_y, within_bounds, tile_number, tile_identifier
        };

    }

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera);

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera);

    ufo::Rectangle GetRectangle(int _x, int _y, Vector2f _frame_size);

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);
};
