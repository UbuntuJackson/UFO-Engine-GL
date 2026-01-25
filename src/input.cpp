#include <SDL3/SDL.h>
#include "../ufo_maths/ufo_maths.h"
#include <unordered_map>
#include "input.h"
#include "../utils/console.h"

void Mouse::ResetTemporaryStates(){
    former_position = position;
    //Reset pressed and released variables as those don't carry over to the next frame
    is_left_button_pressed = false;
    is_left_button_released = false;
    is_right_button_pressed = false;
    is_right_button_released = false;

    is_middle_button_pressed = false;
    is_middle_button_released = false;
}

void Mouse::CheckEvents(SDL_Event& event){

    if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
        if(event.button.button == SDL_BUTTON_LEFT){
            is_left_button_held = true;
            is_left_button_pressed = true;
        }
        if(event.button.button == SDL_BUTTON_RIGHT){
            is_right_button_held = true;
            is_right_button_pressed = true;
        }
        if(event.button.button == SDL_BUTTON_MIDDLE){
            is_middle_button_held = true;
            is_middle_button_pressed = true;
        }
    }
    if(event.type == SDL_EVENT_MOUSE_BUTTON_UP){

        if(event.button.button == SDL_BUTTON_LEFT){
            is_left_button_held = false;
            is_left_button_released = true;
        }
        if(event.button.button == SDL_BUTTON_RIGHT){
            is_right_button_held = false;
            is_right_button_released = true;
        }
        if(event.button.button == SDL_BUTTON_MIDDLE){
            is_middle_button_held = false;
            is_middle_button_released = true;
        }

    }
    if(event.type == SDL_EVENT_MOUSE_MOTION){

        position = Vector2f(event.motion.x, event.motion.y);
        //Console::PrintLine(position);
    }

    delta_position = position-former_position;

    if(event.type == SDL_EVENT_MOUSE_WHEEL){

        mouse_wheel = event.wheel.y;
        Console::PrintLine("SDL EVENT MOUSE WHEEL", mouse_wheel);

    }
    else{
        mouse_wheel = 0;
    }

}

Vector2f Mouse::GetDeltaPosition(){
    float x = 0;
    float y = 0;
    SDL_GetRelativeMouseState(&x, &y);
    return Vector2f(x,y);
}

Vector2f Mouse::GetPosition(){
    float x = 0;
    float y = 0;

    SDL_GetMouseState(&x,&y);

    return Vector2f(x,y);
}

//Keyboard

Keyboard::Key& Keyboard::GetKey(unsigned int _key_id){
    return keys[_key_id];
}

void Keyboard::CheckEvents(SDL_Event& event){
    if(event.key.type == SDL_EVENT_KEY_DOWN){

        if(!keys.count(event.key.key)){
            keys.emplace(event.key.key, Keyboard::Key{false, false, false});
        }
        if(!keys.at(event.key.key).is_held) keys.at(event.key.key).is_pressed = true;
        keys.at(event.key.key).is_held = true;

    }

    if(event.key.type == SDL_EVENT_KEY_UP){

        keys.at(event.key.key).is_held = false;
        keys.at(event.key.key).is_released = true;

    }
}

void Keyboard::ClearPressedAndReleased(){
    //clear keys
    for(auto&& [k,v] : keys){
        v.is_pressed = false;
        v.is_released = false;
    }
}
