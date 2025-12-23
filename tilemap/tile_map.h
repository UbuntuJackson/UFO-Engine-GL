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

    void InitEditorProperties();

    TileMap(Vector2f _);

    void OnSpawn();

    static std::unique_ptr<TileMap> Load(ufo::gc::JsonMap* _layer);

    int GetTileID_AtLevelPosition(Vector2f _position);

    ufo::Rectangle GetFrameFromSpriteSheet(std::string _sprite_key, int _frame, Vector2f _frame_size);

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera);

    ufo::Rectangle GetRectangle(int _x, int _y, Vector2f _frame_size);

    void OnViewProperties(UFOEngineStudio::LevelEditorTab* _level_editor_tab, int _index);

    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor);

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc);
};
