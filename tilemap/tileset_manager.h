#ifndef TILEMAP_H
#define TILEMAP_H
#include <vector>
#include <unordered_map>
#include <string>
#include "tileset_data.h"
#include "../external/olcPixelGameEngine.h"
#include "../json/json.h"
#include "../shapes/rectangle.h"
#include "../ufo_maths/ufo_maths.h"

namespace ufo{
    class Engine;
    class GarbageCollector;

    namespace gc{
        class JsonMap;
    }
}

class Camera;

struct TileCollisionData{
    bool place_free;
    std::vector<int> tiles;
};

class TilesetManager{
public:
    ufo::Engine* engine = nullptr;

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

    int currently_selected_tile = 0;

    std::vector<std::vector<int>> layer_data_sets;
    std::vector<std::vector<int>> foreground_layer_data_sets;

    std::vector<int> tilemap_collision_data;
    TileCollisionData GetTileCollisionData(const ufo::Rectangle& _rectangle, std::string _tileset);

    int number_of_rows;
    int number_of_columns;
    int tile_width;
    int tile_height;
    std::vector<std::string> keys_to_temporary_assets;
    //  firstgid, key
    std::vector<TilesetData> tileset_data;

    TilesetData GetTilesetData(std::string _name);

    void Load(ufo::GarbageCollector* _gc, const ufo::gc::JsonMap* _json);

    void EditorTilesetWidget();

    TilesetManager();

};

#endif
