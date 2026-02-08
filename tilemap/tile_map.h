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
    class TileMapChange{
    public:

        virtual void Do(TileMap* _tile_map){

        }

        virtual void Undo(TileMap* _tile_map){

        }

        virtual void Redo(TileMap* _tile_map){

        }
    };

    class TileMapChange_TileMapSize : public TileMapChange{
    public:
        int top;
        int bottom;
        int left;
        int right;
        TileMapChange_TileMapSize(int _left, int _right, int _bottom, int _top)
        :
        left{_left},
        right{_right},
        bottom{_bottom},
        top{_top}
        {

        }

        void Do(TileMap* _tile_map){
            //The size change is already done in TileMap, probably no need to move it here.

        }

        void Undo(TileMap* _tile_map){
            _tile_map->ResizeBottom(-bottom);
            _tile_map->ResizeRight(-right);
            _tile_map->ResizeLeft(-left);
            _tile_map->ResizeTop(-top);

        }

        void Redo(TileMap* _tile_map){
            _tile_map->ResizeBottom(bottom);
            _tile_map->ResizeRight(right);
            _tile_map->ResizeLeft(left);
            _tile_map->ResizeTop(top);
        }

    };

    class TileMapChange_Paint : public TileMapChange{
    public:
        int left_bound_tile;  // x0
        int lower_bound_tile; // y0
        int right_bound_tile; // x1
        int upper_bound_tile; // y1
        std::vector<int> tiles_before;
        std::vector<int> tiles_after;

        TileMapChange_Paint(int _left_bound_tile, int _lower_bound_tile, int _right_bound_tile, int _upper_bound_tile):
        left_bound_tile{_left_bound_tile},
        lower_bound_tile{_lower_bound_tile},
        right_bound_tile{_right_bound_tile},
        upper_bound_tile{_upper_bound_tile}
        {

        }

        void Do(TileMap* _tile_map){
            for(int yy = lower_bound_tile; yy < upper_bound_tile; yy++){
                for(int xx = left_bound_tile; xx < right_bound_tile; xx++){

                    tiles_before.push_back(_tile_map->tilemap_data_before_change[yy*_tile_map->number_of_columns+xx]);
                    tiles_after.push_back(_tile_map->tilemap_data[yy*_tile_map->number_of_columns+xx]);
                }
            }
        }

        void Undo(TileMap* _tile_map){
            int xx = left_bound_tile;
            int yy = lower_bound_tile;
            int c = right_bound_tile - left_bound_tile;

            Console::PrintLine("Undoing:",_tile_map->current_change, _tile_map->changes.size(), c);

            for(int i = 0; i < (int)tiles_before.size(); i++){

                int tm_x = xx + i%c;
                int tm_y = yy + i/c;

                _tile_map->tilemap_data[tm_x+tm_y*_tile_map->number_of_columns] = tiles_before[i];
            }
        }

        void Redo(TileMap* _tile_map){

            int xx = left_bound_tile;
            int yy = lower_bound_tile;
            int c = right_bound_tile - left_bound_tile;

            Console::PrintLine("Redoing:",_tile_map->current_change, _tile_map->changes.size());

            for(int i = 0; i < (int)tiles_after.size(); i++){

                int tm_x = xx + i%c;
                int tm_y = yy + i/c;

                _tile_map->tilemap_data[tm_x+tm_y*_tile_map->number_of_columns] = tiles_after[i];
            }

            for(const auto& i : tiles_after){
                Console::Print(i,"");
            }
            Console::PrintLine("\n");
        }
    };



    std::vector<std::unique_ptr<TileMapChange>> changes;
    std::vector<int> tilemap_data_before_change;

    int current_change = -1;

    void Do();

    void Undo();

    void Redo();

    int lower_bound_tile;
    int upper_bound_tile;
    int left_bound_tile;
    int right_bound_tile;

    bool visible = true;

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

    void ResizeRight(int _number_of_tiles_to_insert);
    void ResizeLeft(int _number_of_tiles_to_insert);
    void ResizeTop(int _number_of_tiles_to_insert);
    void ResizeBottom(int _number_of_tiles_to_insert);
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

    void OnDrawGizmos(ufo::Graphics* _graphics, Camera* _camera, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera) override;

    ufo::Rectangle GetRectangle(int _x, int _y, Vector2f _frame_size);

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index) override;

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    bool OnUpdateEditorViewportFocus(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab) override;

    void OnAdditionalButtonsForTreeItem() override;

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc) override;
};
