#pragma once
#include "actor.h"
#include "../ufo_maths/ufo_maths.h"
#include "../tilemap/tile_map.h"
#include "camera.h"
#include "graphics.h"
#include "../tilemap/tileset_data.h"
#include "ufo_macros.h"

#ifdef UFO_ENGINE_STUDIO
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

    struct TileCollisionRayCast{
        float hit;
        float angle;
        Vector2f normal;
    };

    ufo::CollisionData GetTileCollisionRayCast(const ufo::TileMap* _tile_map,const std::string& _tileset , const ufo::Ray2& _ray){
        int check_start_tile_x = (int)_ray.Start().x/(int)_tile_map->tile_width-1;
        int check_end_tile_x = (int)_ray.End().x/(int)_tile_map->tile_width+1;

        int check_start_tile_y = (int)_ray.Start().y/(int)_tile_map->tile_height-1;
        int check_end_tile_y = (int)_ray.End().y/(int)_tile_map->tile_height+1;

        if(check_end_tile_x < check_start_tile_x) std::swap(check_end_tile_x, check_start_tile_x);
        if(check_end_tile_y < check_start_tile_y) std::swap(check_end_tile_y, check_start_tile_y);

        std::vector<ufo::CollisionData> ray_collision_data;

        ufo::TilesetData data = level->tileset_manager.GetTilesetData(_tileset);
        if(data.name == ""){
            Console::PrintLine("Tux: Could not find collision tileset");
            return ufo::CollisionData{
                0.0f,
                0.0f,
                Vector2f(0.0f, 0.0f),
                false,
                0.0f, 0.0f
            };
        }

        for(int yy = check_start_tile_y; yy <= check_end_tile_y; yy++){
            for(int xx = check_start_tile_x; xx <= check_end_tile_x; xx++){
                int tile_index = xx+yy*_tile_map->number_of_columns;

                //Trying to prevent out of bounds array access
                if(tile_index >= (int)_tile_map->tilemap_data.size() || tile_index < 0) continue;

                int tile_id = _tile_map->tilemap_data[tile_index] - data.tileset_start_id + 1;

                if(tile_id != EMPTY){
                    /*
                       .__.
                       |__|
                     */
                    if(tile_id == FULL){


                        ufo::CollisionData top = ufo::Maths::RayVsRay(
                            _ray,
                            //Top
                            ufo::Ray2(
                                Vector2f(xx*_tile_map->tile_width,                      yy*_tile_map->tile_height),
                                Vector2f(xx*_tile_map->tile_width+_tile_map->tile_width,yy*_tile_map->tile_height)
                            )
                        );

                        ufo::CollisionData bottom = ufo::Maths::RayVsRay(
                            _ray,
                            //Bottom
                            ufo::Ray2(
                                Vector2f(xx*_tile_map->tile_width,                      yy*_tile_map->tile_height+_tile_map->tile_height),
                                Vector2f(xx*_tile_map->tile_width+_tile_map->tile_width,yy*_tile_map->tile_height+_tile_map->tile_height)
                            )
                        );
                        ufo::CollisionData left = ufo::Maths::RayVsRay(
                            _ray,
                            //Left
                            ufo::Ray2(
                                Vector2f(xx*_tile_map->tile_width,                      yy*_tile_map->tile_height),
                                Vector2f(xx*_tile_map->tile_width,                      yy*_tile_map->tile_height+_tile_map->tile_height)
                            )
                        );

                        ufo::CollisionData right = ufo::Maths::RayVsRay(
                            _ray,
                            //Left
                            ufo::Ray2(
                                Vector2f(xx*_tile_map->tile_width+_tile_map->tile_width,yy*_tile_map->tile_height),
                                Vector2f(xx*_tile_map->tile_width+_tile_map->tile_width,yy*_tile_map->tile_height+_tile_map->tile_height)
                            )
                        );

                        if(top.is_hit) ray_collision_data.push_back(top);
                        if(bottom.is_hit) ray_collision_data.push_back(bottom);
                        if(left.is_hit) ray_collision_data.push_back(left);
                        if(right.is_hit) ray_collision_data.push_back(right);

                    }

                    /*
                          .
                         /|
                        /_|
                    */
                    if(tile_id == SLOPE45_BOTTOM_RIGHT){

                    }

                    /*
                        .
                        |\
                        |_\
                    */
                    if(tile_id == SLOPE45_BOTTOM_LEFT){

                    }
                    if(tile_id == SLOPE_22dot5_RIGHT_1){

                    }
                    if(tile_id == SLOPE_22dot5_RIGHT_2){

                    }

                    if(tile_id == SLOPE_22dot5_LEFT_2){

                    }
                    if(tile_id == SLOPE_22dot5_LEFT_1){

                    }

                }

            }
        }

        std::sort(ray_collision_data.begin(), ray_collision_data.end(), [](const auto& _a, const auto& _b){
            return _a.collision_time < _b.collision_time;
        });

        if(ray_collision_data.empty()){
            return ufo::CollisionData{
                    0.0f,
                    0.0f,
                    Vector2f(0.0f, 0.0f),
                    false,
                    0.0f, 0.0f
            };
        }

        return ray_collision_data[0];
    }

    struct TileCollisionData{
        bool place_free;
        std::vector<int> tiles;
    };

    TileCollisionData GetTileCollisionData(const ufo::TileMap* _tile_map,const std::string& _tileset,const ufo::Rectangle& _rectangle){
        int number_of_tiles = 0;

        std::vector<int> overlapped_tiles;
        bool place_free = true;

        ufo::TilesetData data = level->tileset_manager.GetTilesetData(_tileset);
        if(data.name == ""){
            Console::PrintLine("Tux: Could not find collision tileset");
            return TileCollisionData{place_free,overlapped_tiles};;
        }

        for(int yy = (int)_rectangle.position.y/16 - 1; yy <= (int)_rectangle.position.y/16 + (int)_rectangle.size.y / 16 + 1; yy++){
            for(int xx = (int)_rectangle.position.x/16 - 1; xx <= (int)_rectangle.position.x/16 + (int)_rectangle.size.x / 16 + 1; xx++){

                int tile_index = xx+yy*_tile_map->number_of_columns;

                //Trying to prevent out of bounds array access
                if(tile_index >= (int)_tile_map->tilemap_data.size() || tile_index < 0) continue;

                int tile_id = _tile_map->tilemap_data[tile_index] - data.tileset_start_id + 1;

                if(tile_id != EMPTY){
                    //if(tile_id != FULL) Console::PrintLine("TileId:",tile_id);

                    if(ufo::Maths::RectangleVsRectangle(_rectangle, ufo::Rectangle(Vector2f(xx*16.0f,yy*16.0f), Vector2f(16.0f,16.0f)))){
                        overlapped_tiles.push_back(tile_id);
                        if(tile_id == FULL) place_free = false;

                        /*
                              .
                             /|
                            /_|
                        */
                        if(tile_id == SLOPE45_BOTTOM_RIGHT){

                            /*
                                                          .
                            For leaning part of tile ->  /|
                                                        /_|
                            */

                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 16.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 0.0f))).is_hit)
                            {

                                place_free = false;
                            }

                            /*
                                  .
                                 /| <- For straight part of tile
                                /_|
                            */
                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 0.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 16.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }

                        /*
                            .
                            |\
                            |_\
                        */
                        if(tile_id == SLOPE45_BOTTOM_LEFT){

                            /*
                                .
                                |\ <-Leaning part of triangle.
                                |_\
                            */

                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 0.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 16.0f))).is_hit)
                            {

                                place_free = false;
                            }

                            /*
                                                      .
                            Left part of triangle.->  |\
                                                      |_\
                            */
                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 0.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 16.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }
                        if(tile_id == SLOPE_22dot5_RIGHT_1){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_1");
                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 16.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 8.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }
                        if(tile_id == SLOPE_22dot5_RIGHT_2){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_2");
                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 8.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 0.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }

                        if(tile_id == SLOPE_22dot5_LEFT_2){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_1");
                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 8.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 16.0f))).is_hit)
                            {

                                place_free = false;
                            }
                        }
                        if(tile_id == SLOPE_22dot5_LEFT_1){
                            //Console::PrintLine("SLOPE_22dot5_RIGHT_2");
                            if(
                                ufo::Maths::RayVsRay(
                                    ufo::Ray2(_rectangle.position+Vector2f(0.0f, _rectangle.size.y), _rectangle.position+_rectangle.size),
                                    ufo::Ray2(Vector2f(xx*16.0f,yy*16.0f) + Vector2f(0.0f, 0.0f), Vector2f(xx*16.0f,yy*16.0f) + Vector2f(16.0f, 8.0f))).is_hit)
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

    int max_slope_check_height = 3;
    bool hit_floor = false;
    bool was_hitting_floor = false;
    bool hit_wall = false;
    bool will_jump = false;
    int min_snap_to_floor_height = 10;
    float gravity = 9.82*40.0f;
    Vector2f velocity;
    Vector2f size = {14.0f,24.0f};

    //So that actor slows down on slope. Can be set to 0 without problems
    float speed_loss_by_slope = 3.0f;

    void Collision(const ufo::TileMap* _tile_map, const std::string& _tileset, float _delta_time){

        was_hitting_floor = hit_floor;

        hit_floor = false;
        {
            Vector2f check_for_ground_position = GetRectangle().position+Vector2f(0.0f,1.0f);
            hit_floor = !GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(check_for_ground_position, GetRectangle().size)).place_free;
        }

        if(was_hitting_floor && !hit_floor && velocity.y >= 0.0f){
            Vector2f position_before_snap = local_position;

            //Console::PrintLine("Snap");

            //int snap_height = (int)(std::abs(velocity.x*_delta_time) < min_snap_to_floor_height ? min_snap_to_floor_height : std::abs(velocity.x*_delta_time));

            for(int i = 0; i < min_snap_to_floor_height; i++){
                local_position.y+=1.0f;
                hit_floor = !GetTileCollisionData(_tile_map,_tileset,GetRectangle()).place_free;
                if(hit_floor) local_position.y-=1.0f;
            }
            if(!hit_floor) local_position = position_before_snap;
        }
        else{
            //Console::PrintLine("No snap");
        }

        {

            const float velocity_x_dt = velocity.x * _delta_time;

            hit_wall = false;
            Vector2f wall_check_position = GetRectangle().position + Vector2f(velocity_x_dt, 0.0f);
            bool x_place_free = GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(wall_check_position, GetRectangle().size)).place_free;

            hit_wall = !x_place_free;

            if(!x_place_free){

                ufo::Rectangle current_position_rectangle = GetRectangle();

                while(GetTileCollisionData(_tile_map,_tileset,current_position_rectangle).place_free){
                    current_position_rectangle.position.x += ufo::Maths::PreciseSignFloat(velocity.x);

                }
                current_position_rectangle.position.x-=1.0f;

                bool hit_slope = false;

                //An approximation for whether Tux is actually colliding with a wall or not.
                for(int i = 0; i < 8; i++){

                    ufo::Rectangle slope_checking_rectangle = ufo::Rectangle(
                        current_position_rectangle.position+Vector2f(ufo::Maths::PreciseSignFloat(velocity.x),-i),
                        current_position_rectangle.size
                    );

                    if(GetTileCollisionData(_tile_map,_tileset,slope_checking_rectangle).place_free){
                        hit_slope = true;
                    }

                }

                //Hit floor is a bit of an issue here, because you might want to have tux slide on a slope the frame before they land
                // fine-tuning the conditions here could fix the issue

                for(int i = 0; i < max_slope_check_height*((std::abs(velocity_x_dt) < 1.0f || !hit_floor) ? 1.0f : std::abs(velocity_x_dt)); i++){
                    bool x_slope_place_free = GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(Vector2f(wall_check_position.x, wall_check_position.y-float(i)), GetRectangle().size)).place_free;

                    if(x_slope_place_free && hit_slope){
                        local_position.y-=float(i);
                        hit_wall = false;

                        velocity.x-=ufo::Maths::PreciseSignFloat(velocity.x)*(float(i)*speed_loss_by_slope);
                        break;
                    }

                }


                if(!hit_wall){
                    /*while(!GetTileCollisionData(_tile_map,_tileset,ufo::Rectangle(Vector2f(wall_check_position.x, wall_check_position.y), GetRectangle().size)).place_free){
                        wall_check_position.y-=1.0f;
                    }
                    local_position.y = wall_check_position.y;*/
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
                ufo::Colour(255,255,0), 0.0f);

    }

    void OnLoadDefaultProperties(ufo::gc::JsonMap* _json){
        Actor::OnLoadDefaultProperties(_json);

        try{
            auto j_rectangle = _json->map.at("editor_hitbox")->AsMap();
            float x = j_rectangle.at("x")->AsFloat();
            float y = j_rectangle.at("y")->AsFloat();
            float width = j_rectangle.at("width")->AsFloat();
            float height = j_rectangle.at("height")->AsFloat();
            editor_hitbox = ufo::Rectangle(Vector2f(x, y), Vector2f(width,height));
            get_current_shape = [=](){return ufo::Rectangle(editor_hitbox.position, editor_hitbox.size);};
        } catch(const std::exception& _error){
            Console::PrintLine(__UFO_PRETTY_FUNCTION__,"Could not find properties for "+class_name);
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
    void OnResize(UFOEngineStudio::Editor* _editor, UFOEngineStudio::LevelEditorTab* _level_editor_tab);
#endif //UFO_ENGINE_STUDIO

};
}
