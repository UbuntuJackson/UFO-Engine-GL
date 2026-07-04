#ifndef TILEMAP_H
#define TILEMAP_H
#include <vector>
#include <unordered_map>
#include <string>
#include "tileset_data.h"
#include "../external/olcPixelGameEngine.h"
#include "../utils/json.h"
#include "../shapes/rectangle.h"
#include "../ufo_maths/ufo_maths.h"

namespace UFOEngineStudio{
    class LevelEditorTab;
    class Editor;
}

namespace ufo{
class Engine;
class GarbageCollector;

namespace gc{
    class JsonMap;
}

class Camera;

struct TileCollisionData{
    bool place_free;
    std::vector<int> tiles;
};

struct ManyTiles{
    std::vector<int> tiles;
    int column;
    int row;
    int number_of_columns;
    int number_of_rows;
    int first_selected_tile = 0;
};

class TilesetManager{
public:
    ufo::Engine* engine = nullptr;

    bool in_editor = false;

    const int EMPTY = 0;
    const int FULL = 1;
    const int HALF_BOTTOM = 2;
    const int HALF_TOP = 3;
    const int HALF_LEFT = 4;
    const int HALF_RIGHT = 5;
    const int SLOPE45_BOTTOM_RIGHT = 10;
    const int SLOPE45_BOTTOM_LEFT = 15;
    const int SLOPE_22dot5_RIGHT_1 = 6;
    const int SLOPE_22dot5_RIGHT_2 = 7;

    enum Tools{
        FILL_BUCKET,
        BRUSH,
        ERASER
    };

    Tools tool = BRUSH;

    int currently_selected_tile = 0;

    //Many tiles selected at the same time
    ManyTiles currently_selected_tiles;

    std::vector<std::vector<int>> layer_data_sets;
    std::vector<std::vector<int>> foreground_layer_data_sets;

    std::vector<int> tilemap_collision_data;
    TileCollisionData GetTileCollisionData(const ufo::Rectangle& _rectangle, std::string _tileset);

    int number_of_rows;
    int number_of_columns;
    int tile_width;
    int tile_height;
    std::string tileset_being_recovered;
    std::vector<std::string> keys_to_temporary_assets;
    //  firstgid, key
    std::vector<TilesetData> tileset_data;

    TilesetManager();

    void InitialiseTextures();

    TilesetData GetTilesetData(std::string _name);

#ifdef UFO_ENGINE_STUDIO

    ufo::Rectangle GetSelectionRectangle(){
        float start_x = currently_selected_tiles.column;
        float start_y = currently_selected_tiles.row;

        float end_x = currently_selected_tiles.column + currently_selected_tiles.number_of_columns;
        float end_y = currently_selected_tiles.row + currently_selected_tiles.number_of_rows;

        if(start_y > end_y) std::swap(start_y, end_y);
        if(start_x > end_x) std::swap(start_x, end_x);

        return ufo::Rectangle(Vector2f(start_x,start_y), Vector2f(end_x-start_x, end_y-start_y));

    }

    std::string current_tileset;
    void UpdateSelectedTilesetTile(const TilesetData& _tileset);

    void InitialiseTexturesEditor(UFOEngineStudio::Editor* _editor);

    void AddTileset(const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void RecoverTileset(int _index,const std::string& _path, UFOEngineStudio::LevelEditorTab* _level_editor_tab);

    void EditorTilesetWidget(UFOEngineStudio::LevelEditorTab* _level_editor_tab);
#endif

};

}

#endif
