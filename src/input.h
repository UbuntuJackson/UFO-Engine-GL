#pragma once
#include <unordered_map>
#include <SDL3/SDL.h>
#include "../ufo_maths/ufo_maths.h"


class Mouse{
public:
    Vector2f position;
    Vector2f former_position;
    Vector2f delta_position;

    bool is_left_button_held = false;
    bool is_left_button_pressed = false;
    bool is_left_button_released = false;

    bool is_right_button_held = false;
    bool is_right_button_pressed = false;
    bool is_right_button_released = false;

    bool is_middle_button_held = false;
    bool is_middle_button_pressed = false;
    bool is_middle_button_released = false;

    int mouse_wheel = 0;

    void ResetTemporaryStates();

    void CheckEvents(SDL_Event& event);

    Vector2f GetDeltaPosition();

    Vector2f GetPosition();

};

class Keyboard{
public:
    Keyboard() = default;
    Keyboard(Keyboard&&) = delete;
    Keyboard(const Keyboard&) = delete;

    struct Key{
        bool is_pressed = false;
        bool is_released = false;
        bool is_held = false;
    };

    Key& GetKey(unsigned int _key_id);

    void CheckEvents(SDL_Event& event);

    void ClearPressedAndReleased();

    std::unordered_map<unsigned int, Key> keys;
};
