#pragma once
#include "actor.h"
#include "level.h"

class CollisionGrid : public Actor{
public:
    int column_width = 640;
    int row_height = 640;
    int number_of_divisions = 1;

    std::vector<std::vector<Actor*>> divisions;

    CollisionGrid(Vector2f _) : Actor(_){}

    int GetDivisionFromPosition(Vector2f _position){
        int index = ((int)_position.y/row_height)+(int)column_width/_position.x;

        return index;
    }

    void OnSpawn(){
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

    void OnUpdate(float _delta_time){
        for(auto& division : divisions){
            for(Actor* actor : division){
                int new_division = GetDivisionFromPosition(actor->local_position);
                int former_division = GetDivisionFromPosition(actor->former_local_position);

                if(new_division != former_division){
                    //Unnecessary if the new division is going to be reordered anyway. This could be solved
                    // partly by keeping track of whether things are removed from the division or not.
                    divisions[new_division].push_back(actor);

                    //Somehow push division to list of divisions that should be reordered


                }
            }

            division.clear();
        }



        /*for(const auto& actor : actors){
            GetDivisionFromPosition(actor->local_position).push_back(actor.get());
            }*/
    }

};
