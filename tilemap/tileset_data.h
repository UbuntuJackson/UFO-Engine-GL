#ifndef TILESET_DATA_H
#define TILESET_DATA_H
#include <string>
struct TilesetData{
    std::string name;
    int columns;
    int tileset_start_id;
    float image_width;
    float image_height;
    float tile_width;
    float tile_height;
    int tile_count;
};

#endif