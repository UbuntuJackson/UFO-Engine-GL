#include <vector>
#include <set>
#include "actor.h"
#include "level.h"
#include "../ufo_maths/ufo_maths.h"
#include "collision_grid.h"

namespace ufo{

CollisionGrid::CollisionGrid(Vector2f _) : Actor(_){
    base_class_name = "ufo::CollisionGrid";
    class_name = base_class_name;

}

int CollisionGrid::GetDivisionFromPosition(const Vector2f& _position){
    int index = ((int)_position.y/row_height)*number_of_columns+(int)_position.x/column_width;

    if(!ufo::Maths::RectangleVsPoint(ufo::Rectangle(Vector2f(0.0f, 0.0f), level->size), _position)) return Errors::OUT_OF_BOUNDS;

    return index;
}

void CollisionGrid::OnSpawn(){
    number_of_columns = level->size.x / column_width +1;
    number_of_rows = level->size.y / row_height +1;
    number_of_divisions = number_of_rows*number_of_columns;
    divisions.reserve(number_of_divisions);

    for(int i = 0; i < number_of_divisions; i++){
        divisions.push_back(std::vector<Actor*>{});
    }

    for(auto&& actor : actors){
        int index = GetDivisionFromPosition(actor->local_position);

        if(index != Errors::OUT_OF_BOUNDS) divisions[index].push_back(actor.get());
    }
}

void CollisionGrid::OnUpdate(float _delta_time){

    std::vector<Actor*> actors_to_relocate;

    std::set<int> divisions_to_reset;

    for(const auto& u_actor : actors){

        int new_division = GetDivisionFromPosition(u_actor->local_position);
        int former_division = GetDivisionFromPosition(u_actor->former_local_position);

        //if(new_division == Errors::OUT_OF_BOUNDS) continue;
        //if(former_division == Errors::OUT_OF_BOUNDS) continue;

        if(new_division != former_division || u_actor->is_dead){
            //Unnecessary if the new division is going to be reordered anyway. This could be solved
            // partly by keeping track of whether things are removed from the division or not.
            divisions_to_reset.insert(former_division);


        }

    }

    for(int i : divisions_to_reset){
        for(Actor* actor : divisions[i]){
            int division = GetDivisionFromPosition(actor->local_position);
            if(division != Errors::OUT_OF_BOUNDS) actors_to_relocate.push_back(actor);
        }
        divisions[i].clear();
    }

    for(Actor* actor : actors_to_relocate){
        if(!actor->is_dead) divisions[GetDivisionFromPosition(actor->local_position)].push_back(actor);
    }
}

}
