#pragma once
#include "actor.h"
#include "sprite.h"
#include "colour_circle.h"
#include "colour_rectangle.h"

std::vector<std::unique_ptr<Actor>> GetClasses(){
    std::vector<std::unique_ptr<Actor>> classes;

    classes.push_back(std::make_unique<Actor>(Vector2f(0.0f, 0.0f)));
    classes.push_back(std::make_unique<Sprite>(Vector2f(0.0f, 0.0f)));
    return std::move(classes);
    
}