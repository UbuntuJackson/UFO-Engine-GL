#include <vector>
#include "actor.h"
#include "level.h"
#include "../ufo_maths/ufo_maths.h"
#include "collision_grid.h"

namespace ufo{

CollisionGrid::CollisionGrid(Vector2f _) : Actor(_){
    base_class_name = "ufo::CollisionGrid";
    class_name = base_class_name;

}

int CollisionGrid::GetDivisionFromPosition(Vector2f _position){
    int index = ((int)_position.y/row_height)+(int)column_width/_position.x;

    return index;
}

void CollisionGrid::OnSpawn(){
    int number_of_columns = level->size.x / column_width;
    int number_of_rows = level->size.y / row_height;
    number_of_divisions = number_of_rows*number_of_columns;
    divisions.reserve(number_of_divisions);

    for(int i = 0; i < number_of_divisions; i++){
        divisions.push_back(std::vector<Actor*>{});
        divisions.back().reserve(25);
    }

    for(const auto& actor : actors){
        divisions[GetDivisionFromPosition(actor->local_position)].push_back(actor.get());
    }
}

void CollisionGrid::OnUpdate(float _delta_time){

    std::vector<Actor*> actors_to_relocate;
    actors_to_relocate.reserve(20);

    for(auto& division : divisions){
        for(Actor* actor : division){
            int new_division = GetDivisionFromPosition(actor->local_position);
            int former_division = GetDivisionFromPosition(actor->former_local_position);

            if(new_division != former_division){
                //Unnecessary if the new division is going to be reordered anyway. This could be solved
                // partly by keeping track of whether things are removed from the division or not.

                for(const auto& actor : divisions[new_division]){
                    actors_to_relocate.push_back(actor);
                }

                for(const auto& actor : divisions[former_division]){
                    actors_to_relocate.push_back(actor);
                }

                divisions[new_division].clear();
                divisions[former_division].clear();


            }
        }

        division.clear();
    }

    for(Actor* actor : actors_to_relocate){
        divisions[GetDivisionFromPosition(actor->local_position)].push_back(actor);
    }
}

}
