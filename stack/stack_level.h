#pragma once

#include <cstdint>
#include "../ufo_maths/ufo_maths.h"
#include "stack_actor.h"
#include "stack_sprite.h"
#include "stack_controls.h"
#include "stack_sprite.h"

namespace ufo::stack{

const std::uint8_t max_number_of_actors = 50;

class Level{
public:

    Sprite sprites[max_number_of_actors];
    Controls controls[max_number_of_actors];
    Camera cameras[max_number_of_actors];
    Actor actors[max_number_of_actors];

    std::uint8_t number_of_cameras = 0;
    std::uint8_t number_of_sprites = 0;
    std::uint8_t number_of_controls = 0;
    std::uint8_t number_of_actors = 0;

    void Initialise(){
        /*for(int i = 0; i < max_number_of_actors; i++) sprites[i] = Sprite(Vector2f(0.0f, 0.0f));
        for(int i = 0; i < max_number_of_actors; i++) controls[i] = Controls(Vector2f(0.0f, 0.0f));
        for(int i = 0; i < max_number_of_actors; i++) cameras[i] = Camera(Vector2f(0.0f, 0.0f));
        for(int i = 0; i < max_number_of_actors; i++) actors[i] = Actor(Vector2f(0.0f, 0.0f));*/
    }

    //Non-custom stuff

    std::uint8_t SpawnCamera(Vector2f _position){
        cameras[++number_of_cameras] = Camera(_position);
    }

    std::uint8_t SpawnSprite(Vector2f _position){
        cameras[++number_of_cameras] = Camera(_position);
    }

    //Player

    struct Player{
        Vector2f position;
        Vector2f velocity;
        std::uint8_t sprite; //This works if the indices don't change
        std::uint8_t camera;
    };

    std::uint8_t number_of_players = 0;
    Player players[max_number_of_actors];

    std::uint8_t SpawnPlayer(Vector2f _position){

        Player p = Player{
            _position,
            Vector2f(0.0f, 0.0f),
            SpawnSprite(Vector2f(0.0f, 0.0f)),
            SpawnCamera(Vector2f(0.0f, 0.0f))
        };

        players[++number_of_players] = p;

        return number_of_players;

    }

    void UpdatePlayer(float _delta_time){
        for(int i = 0; i < number_of_players; i++){
            // Update sprite position here?
            //Player logic would be called here, however there's like, no reason to not go with a more object-oriented approach here and
            // make Update a method.
        }

    }

    void Draw(){
        //Determine split screen dimensions if more than one camera.

        for(int c = 0; c < number_of_cameras; c++){
            Camera& camera = cameras[c];

            for(int s = 0; s < number_of_sprites; s++){
                Sprite& sprite = sprites[s];
                if(!sprite.alive) continue;
                //Draw sprites according to their parameters
                //Problem: Positioning for f&cks sake. Need the host position.
            }

            //Camera parameters can be used here to draw the sprites
        }
    }

};

}
