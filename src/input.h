#pragma once

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <unordered_map>

class Mouse{
public:

    bool is_left_button_held = false;
    bool is_left_button_pressed = false;
    bool is_left_button_released = false;

    bool is_right_button_held = false;
    bool is_right_button_pressed = false;
    bool is_right_button_released = false;

    void ResetTemporaryStates(){
        //Reset pressed and released variables as those don't carry over to the next frame
        is_left_button_pressed = false;
        is_left_button_released = false;
        is_right_button_pressed = false;
        is_right_button_released = false;
    }

    void CheckEvents(SDL_Event& event){
        if(event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
            if(event.button.button == SDL_BUTTON_LEFT){
                is_left_button_held = true;
                is_left_button_pressed = true;
            }
            if(event.button.button == SDL_BUTTON_RIGHT){
                is_right_button_held = true;
                is_right_button_pressed = true;
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

        }
    }

    glm::vec2 GetPosition(){
        float x = 0;
        float y = 0;

        SDL_GetMouseState(&x,&y);

        return glm::vec2(x,y);
    }

};

class Keyboard{
public:
    Keyboard(){}
    Keyboard(Keyboard&&) = delete;
    Keyboard(const Keyboard&) = delete;

    struct Key{
        bool is_pressed = false;
        bool is_released = false;
        bool is_held = false;
    };

    Key& GetKey(unsigned int _key_id){
        return keys[_key_id];
    }

    void CheckEvents(SDL_Event& event){
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

    void ClearPressedAndReleased(){
        //clear keys
        for(auto&& [k,v] : keys){
            v.is_pressed = false;
            v.is_released = false;
        }
    }

    std::unordered_map<unsigned int, Key> keys;
};