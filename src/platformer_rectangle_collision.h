#pragma once
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../tilemap/tile_map.h"
#include "camera.h"
#include "graphics.h"
#include "../tilemap/tileset_data.h"

#ifdef UFO_ENGINE_STUDIO
#include "im_vec.h"
#include "viewport_editing_utils.h"
#include "../ufo_engine_studio/level_editor_tab.h"
#include "../ufo_engine_studio/editor.h"
#endif

namespace ufo{

class PlatformerRectangleCollision : public Actor{
public:

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
    const int SLOPE_22dot5_LEFT_1 = 8;
    const int SLOPE_22dot5_LEFT_2 = 9;

    PlatformerRectangleCollision(Vector2f _) : Actor(_){
        base_class_name = "ufo::PlatformerRectangleCollision";
        class_name = base_class_name;
    }

    void OnSpawn(){
        get_current_shape = [this](){
            return editor_hitbox;
        };

    }

    struct TileCollisionData{
        bool place_free;
        std::vector<int> tiles;
    };

    TileCollisionData GetTileCollisionData(const TileMap* _tile_map,const std::string& _tileset,const ufo::Rectangle& _rectangle){
        int number_of_tiles = 0;

        std::vector<int> overlapped_tiles;
        bool place_free = true;

        TilesetData data = level->tileset_manager.GetTilesetData(_tileset);
        if(data.name == ""){
            Console::PrintLine("Tux: Could not find collision tileset");
            return TileCollisionData{place_free,overlapped_tiles};;
        }

        for(int yy = (int)_rectangle.position.y/16 - 1; yy <= (int)_rectangle.position.y/16 + (int)_rectangle.size.y / 16 + 1; yy++){
            for(int xx = (int)_rectangle.position.x/16 - 1; xx <= (int)_rectangle.position.x/16 + (int)_rectangle.size.x / 16 + 1; xx++){

                int tile_id = _tile_map->tilemap_data[xx+yy*_tile_map->number_of_columns] - data.tileset_start_id + 1;

                if(tile_id != EMPTY){
                    //if(tile_id != FULL) Console::PrintLine("TileId:",tile_id);

                    if(ufo::Maths::RectangleVsRectangle(_rectangle, ufo::Rectangle(Vector2f(xx*16.0f,yy*16.0f), Vector2f(16.0f,16.0f)))){
                        overlapped_tiles.push_back(tile_id);
                        if(tile_id == FULL) place_free = false;
                        if(tile_id == SLOPE45_BOTTOM_RIGHT){
                            if(
                                ufo::Maths::RayVsRay(
                                    Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 16.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 0.0f))).is_hit)
                            {

                                place_free = false;
                            }

                            /*if(
                                ufoMaths::RayVsRay(
                                    Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 16.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 0.0f))).is_hit)
                            {

                                place_free = false;
                            }*/
                        }
                        if(tile_id == SLOPE45_BOTTOM_LEFT){
                            if(
                                ufo::Maths::RayVsRay(
                                    Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 0.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 16.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }
                        if(tile_id == SLOPE_22dot5_RIGHT_1){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_1");
                            if(
                                ufo::Maths::RayVsRay(
                                    Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 16.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 8.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }
                        if(tile_id == SLOPE_22dot5_RIGHT_2){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_2");
                            if(
                                ufo::Maths::RayVsRay(
                                    Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 8.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 0.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }

                        if(tile_id == SLOPE_22dot5_LEFT_2){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_1");
                            if(
                                ufo::Maths::RayVsRay(
                                    Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 8.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 16.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }
                        if(tile_id == SLOPE_22dot5_LEFT_1){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_2");
                            if(
                                ufo::Maths::RayVsRay(
                                    Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 0.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 8.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }
                    }
                }

                number_of_tiles++;
            }
        }

        return TileCollisionData{place_free,overlapped_tiles};
    }

    bool VelocityCompare(float _sign, float _location, float _dest){
        if(_sign > 0.0f) return _location < _dest;
        if(_sign < 0.0f) return _location > _dest;
        assert(_sign == 0.0f);
    }

    ufo::Rectangle GetRectangle(){
        ufo::Rectangle rect = get_current_shape();
        rect.position+=GetGlobalPosition();
        return rect;
    }

    std::function<ufo::Rectangle()> get_current_shape;

    const int max_slope_check_height = 3;
    bool hit_floor = false;
    bool hit_wall = false;
    bool will_jump = false;
    const float gravity = 9.82*40.0f;
    Vector2f velocity;
    Vector2f size = {14.0f,24.0f};

    void Collision(const TileMap* _tile_map, const std::string& _tileset, float _delta_time){
        hit_floor = false;
        Vector2f check_for_ground_position = GetRectangle().position+Vector2f(0.0f,1.0f);
        hit_floor = !GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(check_for_ground_position, GetRectangle().size)).place_free;

        {

            const float velocity_x_dt = velocity.x * _delta_time;

            hit_wall = false;
            Vector2f wall_check_position = GetRectangle().position + Vector2f(velocity_x_dt, 0.0f);
            bool x_place_free = GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(wall_check_position, GetRectangle().size)).place_free;

            hit_wall = !x_place_free;

            if(!x_place_free){

                for(int i = 0; i < max_slope_check_height*((std::abs(velocity_x_dt) < 1.0f || !hit_floor) ? 1.0f : std::abs(velocity_x_dt)); i++){
                    bool x_slope_place_free = GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(Vector2f(wall_check_position.x, wall_check_position.y-float(i)), GetRectangle().size)).place_free;

                    if(x_slope_place_free){
                        local_position.y-=float(i);
                        hit_wall = false;
                        //Console::PrintLine("Slowdown");
                        velocity.x-=ufo::Maths::PreciseSignFloat(velocity.x)*(float(i)*3.0f);
                        break;
                    }
                }

                if(!hit_wall){

                }
                else{
                    while(GetTileCollisionData(_tile_map,_tileset,GetRectangle()).place_free){
                        local_position.x += ufo::Maths::PreciseSignFloat(velocity.x);
                    }
                    local_position.x -= ufo::Maths::PreciseSignFloat(velocity.x);
                    velocity.x = 0.0f;
                }

            }

            local_position.x += velocity.x * _delta_time;
        }

        //Logic for y-axis
        {

            velocity.y += gravity * _delta_time;

            Vector2f check_position = GetRectangle().position + Vector2f(0.0f, velocity.y * _delta_time);
            bool y_place_free = GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(check_position, GetRectangle().size)).place_free;

            if(!y_place_free){

                while(GetTileCollisionData(_tile_map,_tileset,GetRectangle()).place_free){
                    local_position.y += ufo::Maths::PreciseSignFloat(velocity.y);
                }
                local_position.y -= ufo::Maths::PreciseSignFloat(velocity.y);
                velocity.y = 0.0f;
            }

            local_position.y += velocity.y * _delta_time;

        }
    }

    void OnDraw(ufo::Graphics* _graphics, Camera* _camera){
        //return;

            auto hit_box = GetRectangle();
            _graphics->DrawRectangleExtra(
                _camera->Transform(hit_box.position),
                hit_box.size*_camera->scale,
                Vector2f(0.0f, 0.0f),
                Vector2f(1.0f, 1.0f),0.0f,
                ufo::Colour(255,255,0));

    }

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json){

        try{
            auto j_rectangle = _json->map.at("editor_hitbox")->AsMap();
            float widget_x = j_rectangle.at("x")->AsFloat();
            float widget_y = j_rectangle.at("y")->AsFloat();
            float widget_w = j_rectangle.at("width")->AsFloat();
            float widget_h = j_rectangle.at("height")->AsFloat();
            get_current_shape = [=](){return ufo::Rectangle(Vector2f(widget_x, widget_y), Vector2f(widget_w,widget_h));};
        } catch(const std::exception& _error){
            Console::PrintLine("[UFO-Engine] GenericGenerator: Could not find properties for "+class_name);
        }

    }

    ufo::gc::JsonMap* GetAsJson(ufo::GarbageCollector* _gc){

        ufo::gc::JsonMap* parent_class_as_json = Actor::GetAsJson(_gc);

        /*ufo::Rectangle rectangle = GetRectangle();

        auto j_rectangle = _gc->New<ufo::gc::JsonMap>();
        j_rectangle->map.emplace("x", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.position.x));
        j_rectangle->map.emplace("y", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.position.y));
        j_rectangle->map.emplace("w", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.size.x));
        j_rectangle->map.emplace("h", _gc->New<ufo::gc::JsonNumber>(editor_hitbox.size.y));

        parent_class_as_json->map.emplace("editor_hitbox", j_rectangle);*/
        return parent_class_as_json;
    }
#ifdef UFO_ENGINE_STUDIO
    PartsOfRectangle part_of_rectangle_resized_in_editor = PartsOfRectangle::NONE;
    void OnUpdateEditorViewport(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);
#endif //UFO_ENGINE_STUDIO

};
}
