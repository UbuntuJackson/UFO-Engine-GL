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

std::vector<int> CollisionGrid::GetDivisionsForRectangle(const ufo::Rectangle& _rect){

    std::vector<int> divisions;

    int start_section_x = (int)(_rect.position.x/column_width);
    int end_section_x = (int)((_rect.position.x+_rect.size.x)/column_width);

    int start_section_y = (int)(_rect.position.y/row_height);
    int end_section_y = (int)((_rect.position.y+_rect.size.y)/row_height);

    for(int yy = start_section_y; yy <= end_section_y; yy++){
        for(int xx = start_section_x; xx <= end_section_x; xx++){
            int index = GetDivisionFromPosition(Vector2f(xx*column_width, yy*row_height));
            if(index != Errors::OUT_OF_BOUNDS) divisions.push_back(index);
        }
    }
    return divisions;

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
        std::vector<int> divisions_for_actor = GetDivisionsForRectangle(actor->GetRectangle());
        Console::PrintLine("Actor rect",actor->editor_name,actor->GetRectangle().position,actor->GetRectangle().size);
        for(const int index : divisions_for_actor){
            Console::PrintLine("Actor added to grid",actor->editor_name);
            divisions[index].push_back(actor.get());
        }
    }
}

void CollisionGrid::OnUpdate(float _delta_time){

    std::set<int> divisions_to_reset;

    for(const auto& u_actor : actors){
        std::vector<int> new_divisions_for_actor = GetDivisionsForRectangle(u_actor->GetRectangle());
        std::vector<int> former_divisions_for_actor = GetDivisionsForRectangle(u_actor->former_rectangle);

        //if(new_division == Errors::OUT_OF_BOUNDS) continue;
        //if(former_division == Errors::OUT_OF_BOUNDS) continue;

        if(new_divisions_for_actor != former_divisions_for_actor || u_actor->is_dead){
            Console::PrintLine(u_actor->GetRectangle().position, u_actor->former_rectangle.position);

            //Unnecessary if the new division is going to be reordered anyway. This could be solved
            // partly by keeping track of whether things are removed from the division or not.
            for(const int division : former_divisions_for_actor) divisions_to_reset.insert(division);
            for(const int division : new_divisions_for_actor) divisions_to_reset.insert(division);


        }

    }

    std::set<Actor*> actors_to_relocate;

    for(int i : divisions_to_reset){
        for(Actor* actor : divisions[i]){
            actors_to_relocate.insert(actor);
        }
        divisions[i].clear();
    }

    if(!actors_to_relocate.empty()) Console::PrintLine(actors_to_relocate.size());

    for(Actor* actor : actors_to_relocate){
        if(!actor->is_dead){
            for(const int division : GetDivisionsForRectangle(actor->GetRectangle())){
                divisions[division].push_back(actor);
            }
        }
    }
}

}
