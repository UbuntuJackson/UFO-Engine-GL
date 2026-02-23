#pragma once

#include <bitmap_collision_body.h>
#include <level.h>
#include <ufo_maths.h>
#include <animation.h>
#include <animated_sprite_reference.h>
#include <single_keyboard.h>
#include <console.h>
#include <sprite_reference.h>
#include "pingus_level.h"
#include <ufo_maths.h>
#include <rectangle.h>
#include <mouse.h>
#include <timer.h>
#include <widget_sprite_reference.h>
#include "goal.h"
#include "honey_coin.h"
#include <colour_utils.h>
#include "item_select_menu.h"
#include "moving_solid.h"
#include <olcPixelGameEngine.h>

/// @spawn;
class Pingu : public BitmapCollisionBody{
public:
    //In some levels pingus can be antimatter
    bool is_anti_matter = false;

    Animation* anim = nullptr;

    //Which direction the pingu is facing. This will dictate the velocity in the x-axis
    float face_direction = 1.0f;

    const float initial_jump_velocity_y = 300.0f;
    const float initial_jump_velocity_x = 190.0f;
    const float initial_jump_velocity_when_hitting_slope_y = 400.0f;
    const float initial_jump_velocity_when_hitting_slope_x = 80.0f;
    bool hit_slope_when_jumping = false;

    //Did the pingu hit floor last frame?
    bool hit_floor_last_frame = false;
    bool snap_to_ground_enabled = true;
    PingusLevel* level = nullptr;

    Timer build_timer;
    Timer fall_timer;
    Timer driller_timer;
    Timer wait_timer;

    //Unless the pingu is in a special state it will either be state_walk or state_fall
    bool is_in_special_state = false;

    //Did the pingu explode?
    bool exploded = false;
    int steps = 0;

    //Used for Builder
    Vector2f build_location;

    //Did the pingu reach the goal?
    bool is_rescued = false;

    //These are all possible states the pingu can have
    enum States{
        WALK,
        FALL,
        BUILD,
        EXPLODE,
        DIE_BY_FALL,
        PARACHUTE,
        BLOCKER,
        CLIMBER,
        FALL_AFTER_CLIMBER,
        DRILLER,
        WAIT,
        JUMP
    };

    //This variable is used to identify which state the pingu is in, however it does not SET the state
    //If you want to set the state you better use one of the already existing methods and set state = state_SOMETHING
    int what_is_current_state = States::WALK;

    //This runs whenever the pingu is walking
    std::function<void()> state_walk = [this](){
        what_is_current_state = States::WALK;
        Walk();
    };

    //This runs whenever the pingu is falling
    std::function<void()> state_fall = [this](){
        what_is_current_state = States::FALL;
        Fall();
    };

    //This runs whenever the pingu is building a staircase
    std::function<void()> state_build = [this](){
        what_is_current_state = States::BUILD;
        Build();
    };

    //This runs when the pingu explodes
    std::function<void()> state_explode = [this](){
        what_is_current_state = States::EXPLODE;
        Explode();
    };

    //This runs when you fall from too high
    std::function<void()> die_by_fall = [this](){
        what_is_current_state = States::DIE_BY_FALL;
        DieByFall();
    };

    //This is so you can remove the parachute sprite once the parachuter lands
    int parachute_sprite_id = -1;

    //This runs whenever the pingu falls with a parachute on
    std::function<void()> state_parachute = [this](){
        what_is_current_state = States::PARACHUTE;
        Parachute();
    };

    //bool is_already_overlapping_blue = false;
    Vector2f blocker_collision_position;
    //This runs when a pingu has a blocker
    std::function<void()> state_blocker = [this](){
        what_is_current_state = States::BLOCKER;
        Blocker();
    };

    //This runs whenever a pingu that has a climber action detects a wall in it's climbing direction
    std::function<void()> state_climber = [this](){
        what_is_current_state = States::CLIMBER;
        Climber();
    };

    //Pingu will enter this state when the wall abruptly ends during a climb, but didn't hit a ceiling
    std::function<void()> state_fall_after_climber = [this](){
        what_is_current_state = States::FALL_AFTER_CLIMBER;
        FallAfterClimber();
    };

    //This runs whenever a pingu drills
    std::function<void()> state_driller = [this](){
        Driller();
    };

    std::function<void()> state_wait = [this](){
        what_is_current_state = States::WAIT;
        Wait();
    };

    //This runs when you fall from too high
    std::function<void()> state_jump = [this](){
        what_is_current_state = States::JUMP;
        Jump();
    };

    std::function<void()> state = state_walk;

    float max_slope_height = 3.0f;

    Pingu(olc::vf2d _local_position) : BitmapCollisionBody(_local_position, "box_12_24", "solid"){

    }
    void OnLevelEnter(Level* _level){

        BitmapCollisionBody::OnLevelEnter(_level);

        anim = AddChild<Animation>(Vector2f(6.0f, 19.0f));
        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_walk",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            20.0f
        ));
        anim->SetAnimation("pingu_walk");

        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_walk_parachute",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            20.0f
        ));

        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_explode",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            10.6f
        ));

        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_fall_death",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            10.6f
        ));

        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_fall",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            10.6f
        ));

        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_blocker",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            10.6f
        ));

        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_parachute",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            10.6f
        ));

        anim->AddAnimationState(AnimatedSpriteReference(
            "pingu_driller",
            Vector2f(0.0f, 0.0f),
            Vector2f(16.0f,24.0f),
            Vector2f(32.0f, 32.0f),
            Vector2f(1.0f,1.0f),
            0.0f,
            20.0f
        ));

        if(Engine::Get().all_shapes_visible){
            AddChild<SpriteReference>(
                mask,Vector2f(0.0f, 0.0f),
                Vector2f(0.0f, 0.0f),
                Vector2f(12.0f, 24.0f),
                Vector2f(1.0f, 1.0f),
                0.0f
            )->tint.a = 100;
        }

        level = dynamic_cast<PingusLevel*>(_level);

        level->released_pingus++;
        level->pingu_handles_all_pingus.push_back(this);

        SetZIndex(1);

    }

    void OnStart(Level* _level){
        if(is_anti_matter){
            level->anti_matter_pingus.push_back(this);
        }
    }

    bool has_parachute = false;

    void Parachute(){
        fall_timer.Start(5000000.0f);
        fall_timer.Stop();
        anim->SetAnimation("pingu_parachute");
        velocity.y = 20.0f;

        if(hit_floor){
            has_parachute = false;
            is_in_special_state = false;
            level->QueueForPurge(parachute_sprite_id);
            parachute_sprite_id = -1;
        }
    }

    void DieByFall(){
        velocity.x = 0.0f;
        if(anim->current_animation_state->key != "pingu_fall_death") anim->SetAnimation("pingu_fall_death");
        if(anim->cycle_count > 0){
            QueueForPurge();
        }
    }

    void Walk(){
        anim->current_animation_state->scale.x = face_direction;
        velocity.x = face_direction * 30.0f;

        if(has_climber && hit_wall){

            state = state_climber;
            is_in_special_state = true;

        }

        if(has_climber) anim->current_animation_state->scale.x = climbing_direction;

        if(!has_parachute) anim->SetAnimation("pingu_walk");
        else anim->SetAnimation("pingu_walk_parachute");

        if(fall_timer.GetTimeLeft() <= 0.0f){
            if(IsOverlapping(level,mask_decal,solid_layer,local_position+Vector2f(0.0f,2.0f),olc::MAGENTA)){

                for(auto&& [k,v] : level->asset_manager.GetDecals()){

                    if(k == "bg") continue;

                    for(int yy = -32+12; yy < 32+12; yy++){
                        for(int xx = -32+6; xx < 32+6; xx++){
                            if(ufo::Maths::Distance2(local_position+Vector2f(xx,yy), local_position+Vector2f(6.0f,12.0f)) <= 32.0f){
                                if(ufo::Maths::Distance2(local_position+Vector2f(xx,yy), local_position+Vector2f(6.0f,12.0f)) > 28.0f
                                    && v->sprite->GetPixel(local_position+Vector2f(xx,yy)).a != 0
                                ){
                                    olc::Pixel c = v->sprite->GetPixel(local_position+Vector2f(xx,yy));
                                    v->sprite->SetPixel(local_position+Vector2f(xx,yy),olc::Pixel(25,25,25));
                                }
                                else v->sprite->SetPixel(local_position+Vector2f(xx,yy),olc::Pixel(0,0,0,0));
                            }

                        }
                    }

                    //Requesting update to avoid potential stutter when pingu modifies terrain while fasting forward
                    //Implying an increase in updates of decal per frame (x4)
                    v->Update();
                }

            }

            state = die_by_fall;

            is_in_special_state = true;

        }

        //To make sure the timer does not reach below zero when entering fall state again.
        fall_timer.Start(5000000.0f);
        fall_timer.Stop();
    }

    void Fall(){
        if(!fall_timer.is_started) fall_timer.Start(1975.0f);
        anim->SetAnimation("pingu_fall");
        velocity.x = 0.0f;
        velocity.y = 100.0f;
        if(has_parachute){
            state = state_parachute;
            ResetAllStates();

            parachute_sprite_id = AddChild<SpriteReference>("parachute",
                Vector2f(-22.0f, -48.0f),
                Vector2f(0.0f, 0.0f),
                Vector2f(59.0f, 66.0f),
                Vector2f(1.0f, 1.0f),
                0.0f
            )->GetID();

            is_in_special_state = true;
        }
    }

    void SetStateWait(){
        ResetAllStates();
        what_is_current_state = States::WAIT;
        is_in_special_state = true;
        state = state_wait;
        wait_timer.Start(3000.0f);
        anim->SetAnimation("pingu_blocker");
    }

    void Build(){
        float step_width = 3.0f;
        int number_of_steps = 25;

        snap_to_ground_enabled = false;

        if(steps == number_of_steps || hit_wall || hit_slope || IsOverlappingHead(local_position,olc::WHITE)){

            if(steps == number_of_steps) SetStateWait();
            else{
                is_in_special_state = false;
                snap_to_ground_enabled = true;
            }

            return;
        }

        bool reached_location = false;

        float destination_x = build_location.x + face_direction * step_width * steps;
        if(face_direction > 0.0f){
            if(local_position.x < destination_x) velocity.x = 6.0f;
            else reached_location = true;
        }

        if(face_direction < 0.0f){
            if(local_position.x > destination_x) velocity.x = -6.0f;
            else reached_location = true;
        }

        if(reached_location){

            build_timer.Start(200.0f);

            level->should_update_level_decals = true;

            olc::Decal* dec = level->asset_manager.GetDecal("solid");
            olc::Decal* dec_mg = nullptr;
            if(level->asset_manager.decals.count("mg")) dec_mg = level->asset_manager.GetDecal("mg");

            for(int yy = 0; yy < 2; yy++){
                for(int xx = 6; xx < 6+(int)step_width*2; xx++){

                    dec->sprite->SetPixel(build_location+Vector2f(xx+step_width*steps*face_direction,yy+24.0f-2.0f-steps*2),olc::Pixel(255,0,0,255));

                    if(dec_mg){
                        dec_mg->sprite->SetPixel(build_location+Vector2f(xx+step_width*steps*face_direction,yy+24.0f-2.0f-steps*2),olc::Pixel(200,50,50,255));
                    }

                    //level->level_decals.at("mg")->sprite->SetPixel(local_position+Vector2f(xx,yy),olc::Pixel(255,0,0,255));

                }
            }

            steps++;

        }

    }

    void Explode(){
        velocity.x = 0.0f;

        if(anim->current_animation_state->key == "pingu_explode"){

            if(anim->current_animation_state->current_frame_index > 4.9 && !exploded){

                for(int yy = -32+12; yy < 32+12; yy++){
                    for(int xx = -32+6; xx < 32+6; xx++){
                        for(auto&& [k,v] : game->asset_manager.GetDecals()){
                            if(k == "bg") continue;

                            auto solid_decal = level->asset_manager.GetDecal(solid_layer);
                            if(CompareColour(solid_decal->sprite->GetPixel(local_position+Vector2f(xx,yy)), olc::VERY_DARK_GREY)) continue;
                            if(CompareColour(solid_decal->sprite->GetPixel(local_position+Vector2f(xx,yy)), olc::BLUE)) continue;

                            if(ufo::Maths::Distance2(local_position+Vector2f(xx,yy), local_position+Vector2f(6.0f,12.0f)) <= 33.0f){
                                if(ufo::Maths::Distance2(local_position+Vector2f(xx,yy), local_position+Vector2f(6.0f,12.0f)) > 28.0f
                                    && v->sprite->GetPixel(local_position+Vector2f(xx,yy)).a != 0
                                ){
                                    olc::Pixel c = v->sprite->GetPixel(local_position+Vector2f(xx,yy));
                                    v->sprite->SetPixel(local_position+Vector2f(xx,yy),olc::Pixel(25,25,25));
                                }
                                else v->sprite->SetPixel(local_position+Vector2f(xx,yy),olc::Pixel(0,0,0,0));
                            }
                        }

                    }
                }
                for(auto&& [k,v] : game->asset_manager.GetDecals()){
                    v->Update();
                }

                exploded = true;

            }
            if(anim->cycle_count > 0){
                QueueForPurge();
            }

        }
    }

    bool is_already_blocker = false;

    void Blocker(){
        velocity.x = 0.0f;

        //This should move the blocker collision if blocker is affected by for example gravity.
        if(local_position != former_position){
            RemoveBlockerCollision(blocker_collision_position);
            ActivateBlockerCollision();
        }

        if(!hit_floor){
            RemoveBlockerCollision(blocker_collision_position);
            item_walk();
        }

        is_already_blocker = true;
    }

    float climbing_direction = 1.0f;

    void Climber(){

        velocity.y = -50.0f;

        anim->current_animation_state->scale.x = climbing_direction;
        anim->current_animation_state->rotation = climbing_direction*90.0f * 180.0f/3.1415f;

        snap_to_ground_enabled = false;

        if(
            hit_ceiling ||
            (!IsOverlappingFeet(local_position + Vector2f(climbing_direction * 1.0f, 0.0f), olc::WHITE))
            //!IsOverlappingSolid(local_position + Vector2f(climbing_direction * 1.0f, 0.0f))
        ){
            anim->current_animation_state->rotation = 0.0f;
            if(!hit_ceiling){
                Console::PrintLine("Climber hit wall", hit_wall);
                state = state_fall_after_climber;
            }
            else{
                Console::PrintLine("Climber hit ceiling.");
                climbing_direction *= -1.0f;
                is_in_special_state = false;
                state = state_walk;
                snap_to_ground_enabled = true;
            }
            velocity.y = 0.0f;

            face_direction = climbing_direction;
        }
    }

    void FallAfterClimber(){

        velocity.x = face_direction * 30.0f;

        Console::Print("FallAfterClimber\n");

        if(hit_wall){
            climbing_direction *= -1.0f;
            face_direction = climbing_direction;
            state = state_fall;
            is_in_special_state = false;
            snap_to_ground_enabled = true;
            return;
        }

        if(hit_floor){
            state = state_walk;
            is_in_special_state = false;
            snap_to_ground_enabled = true;
        }
    }

    void Driller(){
        velocity.x = 0.0f;
        velocity.y = 10.0f;
        anim->SetAnimation("pingu_driller");
        if(driller_timer.GetTimeLeft() < 0.0f){
            driller_timer.Start(driller_pace);

            for(int yy = -32+12; yy < 32+12; yy++){
                for(int xx = -32+6; xx < 32+6; xx++){
                    auto solid_decal = level->asset_manager.GetDecal(solid_layer);
                    if(CompareColour(solid_decal->sprite->GetPixel(local_position+Vector2f(xx,yy)), olc::VERY_DARK_GREY)) continue;
                    if(CompareColour(solid_decal->sprite->GetPixel(local_position+Vector2f(xx,yy)), olc::BLUE)) continue;

                    for(auto&& [k,v] : game->asset_manager.GetDecals()){
                        if(k == "bg") continue;
                        if(ufo::Maths::Distance2(local_position+Vector2f(xx,yy), local_position+Vector2f(6.0f,12.0f)) <= 14.0f){
                            if(ufo::Maths::Distance2(local_position+Vector2f(xx,yy), local_position+Vector2f(6.0f,12.0f)) > 10.0f
                                && v->sprite->GetPixel(local_position+Vector2f(xx,yy)).a != 0
                            ){
                                olc::Pixel c = v->sprite->GetPixel(local_position+Vector2f(xx,yy));
                                v->sprite->SetPixel(local_position+Vector2f(xx,yy),olc::Pixel(25,25,25));
                            }
                            else v->sprite->SetPixel(local_position+Vector2f(xx,yy),olc::Pixel(0,0,0,0));
                        }
                    }

                }
            }

            for(auto&& [k,v] : game->asset_manager.GetDecals()){
                v->Update();
            }

        }
        if(!hit_floor || IsOverlappingFeet(local_position+Vector2f(0.0f,1.0f),olc::VERY_DARK_GREY)){
            state = state_walk;
            is_in_special_state = false;
            snap_to_ground_enabled = true;
        }
    }

    void Wait(){

        velocity.x = 0.0f;
        if(!hit_floor || wait_timer.GetTimeLeft() < 0.0f){
            state = state_walk;
            wait_timer.Stop();
            is_in_special_state = false;
            snap_to_ground_enabled = true;
        }
    }

    void Jump(){
        /*if(!hit_slope_when_jumping) velocity.x = initial_jump_velocity_x * face_direction;
        else velocity.x = initial_jump_velocity_when_hitting_slope_x * face_direction;

        anim->current_animation_state->scale.x = face_direction;

        if((IsOverlappingFeet(local_position + Vector2f(face_direction, 0.0f), Graphics::WHITE) ||
            IsOverlappingFeet(local_position + Vector2f(face_direction, 0.0f), Graphics::RED) ||
            IsOverlappingFeet(local_position + Vector2f(face_direction, 0.0f), olc::VERY_DARK_GREY)
            && hit_floor && velocity.y > 0.0f
            )
        )
        {
            hit_slope_when_jumping = true;
            velocity.y = -initial_jump_velocity_when_hitting_slope_y;

        }*/

        velocity.x = initial_jump_velocity_x * face_direction;

        velocity.y += 500.0f * Engine::Get().GetDeltaTime();
        if(hit_floor && velocity.y > 0.0f){
            hit_slope_when_jumping = false;
            state = state_walk;
            is_in_special_state = false;
            snap_to_ground_enabled = true;
        }
    }

    std::function<bool()> item_walk = [this](){
        ResetAllStates();

        int former_state = what_is_current_state;
        what_is_current_state = States::WALK;

        is_in_special_state = false;
        snap_to_ground_enabled = true;
        state = state_walk;

        return true;
    };

    bool has_climber = false;

    std::function<bool()> item_climber = [this](){
        if(
            what_is_current_state == States::BLOCKER ||
            what_is_current_state == States::FALL ||
            what_is_current_state == States::PARACHUTE ||
            what_is_current_state == States::CLIMBER ||
            has_climber
        ) return false;

        has_climber = true;
        climbing_direction = face_direction;

        return true;
    };

    void ResetAllStates(){
        is_in_special_state = false;
        snap_to_ground_enabled = true;

        //Blocker
        RemoveBlockerCollision(blocker_collision_position);

        steps = 0;
    }

    std::function<bool()> item_block = [this](){
        if(
            what_is_current_state == States::FALL ||
            what_is_current_state == States::PARACHUTE ||
            what_is_current_state == States::BLOCKER
        ) return false;

        ResetAllStates();

        anim->SetAnimation("pingu_blocker");
        anim->frame_counter = 0.0f;

        ActivateBlockerCollision();

        state = state_blocker;

        is_in_special_state = true;
        //snap_to_ground_enabled = false;
        return true;
    };

    std::function<bool()> item_blow_up = [this](){
        if(what_is_current_state == States::EXPLODE) return false;

        ResetAllStates();

        what_is_current_state = States::EXPLODE;

        anim->SetAnimation("pingu_explode");
        anim->frame_counter = 0.0f;

        state = state_explode;

        is_in_special_state = true;
        return true;
    };

    std::function<bool()> item_build = [this](){
        if(!hit_floor || what_is_current_state == States::BUILD || what_is_current_state == States::EXPLODE) return false;

        ResetAllStates();

        //Doesn't hurt to make sure the wait timer is stopped
        wait_timer.Stop();
        steps = 0;

        int former_state = what_is_current_state;
        what_is_current_state = States::BUILD;
        ResetAllStates();

        Console::PrintLine("Pingu::item_build");

        anim->SetAnimation("pingu_walk");

        build_location = local_position;

        state = state_build;
        build_timer.Start(200.0f);

        is_in_special_state = true;

        return true;
    };

    std::function<bool()> item_parachute = [this](){

        if(has_parachute || what_is_current_state == States::PARACHUTE) return false;

        has_parachute = true;

        return true;
    };

    float driller_pace = 50.0f;

    std::function<bool()> item_driller = [this](){
        if(!hit_floor && what_is_current_state == States::DRILLER) return false;

        ResetAllStates();
        int former_state = what_is_current_state;
        what_is_current_state = States::DRILLER;

        is_in_special_state = true;
        state = state_driller;
        driller_timer.Start(driller_pace);
        //snap_to_ground_enabled = false;
        return true;
    };

    std::function<bool()> item_jump = [this](){
        if(!hit_floor || what_is_current_state == States::JUMP) return false;
        ResetAllStates();

        int former_state = what_is_current_state;
        what_is_current_state = States::JUMP;

        is_in_special_state = true;
        state = state_jump;

        velocity.y = -initial_jump_velocity_y;
        snap_to_ground_enabled = false;

        anim->SetAnimation("pingu_fall");

        return true;
    };

    std::vector<std::function<bool()>> items = {
        item_blow_up,
        item_build,
        item_parachute
    };

    int current_item = 2;

    void ActivateBlockerCollision(){
        blocker_collision_position = local_position;
        int width = 2;

        auto dec = game->asset_manager.GetDecal(solid_layer);

        for(int yy = 0; yy < 6; yy++){
            for(int xx = 5; xx < 5+width; xx++){

                Vector2f place_pos = local_position+Vector2f(xx,24.0f-yy);

                olc::Pixel p = dec->sprite->GetPixel(place_pos);

                //This is not {0,0,0,0} for some reason but instead {115,121,121,0}

                if(dec->sprite->GetPixel(place_pos).a == 0){
                    dec->sprite->SetPixel(place_pos,olc::BLUE);
                }

            }
        }

        level->should_update_level_decals = true;
    }

    //Resets from blocker state
    void RemoveBlockerCollision(Vector2f _position_for_blue_collision_pixles){
        int width = 2;
        for(int yy = 0; yy < 6; yy++){
            for(int xx = 5; xx < 5+width; xx++){

                auto dec = level->asset_manager.GetDecal(solid_layer);

                Vector2f place_pos = _position_for_blue_collision_pixles+Vector2f(xx,24.0f-yy);
                if(dec->sprite->GetPixel(place_pos) == olc::BLUE) dec->sprite->SetPixel(place_pos,olc::Pixel(0,0,0,0));
                level->should_update_level_decals = true;

            }
        }
    }

    bool OnSelectionIteration(){
        level->at_least_one_pingu_active = true;

        if(!level->pingu_selected_this_frame && RectangleVsPoint(ufo::Rectangle(local_position, Vector2f(12.0f,24.0f)),level->GetActiveCamera()->TransformScreenToWorld(Mouse::Get().GetPosition()))){
            anim->current_animation_state->tint = olc::GREEN;

            if(Mouse::Get().GetLeftButton().is_pressed){

                level->item_select_menu->items[level->item_select_menu->selected_index](this);

                return true;

            }
        }
        else{
            anim->current_animation_state->tint = olc::WHITE;
        }

        return false;
    }

    void PinguUpdate(){
        //Can be uncommented for performance testing
        //OnSelectionIteration();

        //Checking against antimatter pingus
        if(!is_anti_matter && what_is_current_state != States::EXPLODE){
            for(const auto& anti_matter_pingu : level->anti_matter_pingus){
                if(
                    RectangleVsRectangle(
                        ufo::Rectangle(local_position, Vector2f(12.0f,24.0f)),
                        ufo::Rectangle(anti_matter_pingu->local_position, Vector2f(12.0f,24.0f))
                    ) &&
                    anti_matter_pingu->what_is_current_state != States::EXPLODE
                ){

                    item_blow_up();
                    anti_matter_pingu->item_blow_up();
                    anim->current_animation_state->current_frame_index = 4.9f;
                    anim->frame_counter = 4.9f;
                    anti_matter_pingu->anim->current_animation_state->current_frame_index = 4.9f;
                    anti_matter_pingu->anim->frame_counter = 4.9f;

                }
            }
        }

        //If pingu is not in special state it will always switch between state_walk and state_fall
        if(!is_in_special_state){
            if(hit_floor){
                state = state_walk;
                for(const auto& goal : level->goals){
                    if(ufo::Maths::RectangleVsRectangle(ufo::Rectangle(local_position, Vector2f(12.0f,24.0f)),goal->shape)){
                        is_rescued = true;
                    }
                }
            }
            else{
                state = state_fall;
            }
        }

        //If the pingu falls off level
        if(local_position.y > level->level_size.y) QueueForPurge();

        for(const auto& honey_coin : level->honey_coin_handles){
            if(ufo::Maths::RectangleVsCircle(ufo::Rectangle(local_position, Vector2f(12.0f,24.0f)),honey_coin->shape)){
                level->honey_coin_hud->current_frame_index = 0.0f;
                honey_coin->QueueForPurge();
            }
        }

        //Runs the current state
        state();

        //bool is_already_overlapping_blue = IsOverlappingFeet(local_position,olc::BLUE);

        //Attempt to create depth when pingus climb up semisolid and overlap pingu on lower level
        if(int(former_position.y) != int(local_position.y)) level->should_resort_after_z_index = true;

        former_position = local_position;

        PinguCollision();

        //Checking against BLUE which means there's a blocker pingu in the way
        //This part was changed in order to ensure pingu is never inside another blocker. Tried checking all blue collision beforehand with
        //PinguSelectionManager but it isn't very functional somehow. What has been tried so far is to set is_already_overlapping_blue before
        //MovingSolid AND pingu moves. This involves is_already_overlapping_blue a member variable of Pingu. Hence resolving is a sturdier solution.
        if(IsOverlappingFeet(local_position,olc::BLUE) && what_is_current_state != States::BLOCKER /*&& !is_already_overlapping_blue*/){
            while(IsOverlappingFeet(local_position,olc::BLUE) && !IsOverlappingSolid(local_position) && velocity.x != 0.0f){
                local_position.x -= ufo::Maths::Sign(velocity.x);
                Console::PrintLine("While loop in PinguUpdate");
            }
            if(IsOverlappingSolid(local_position)) local_position.x += ufo::Maths::Sign(velocity.x);
            face_direction *= -1.0f;
        }

        //If hitting wall under normal circumstances.
        if(hit_wall && !has_climber){
            face_direction *= -1.0f;

        }
    }

    void OnPaused(){

        build_timer.Pause();
        wait_timer.Pause();
        fall_timer.Pause();

    }

    void OnUpdate(){
        build_timer.Resume();
        fall_timer.Resume();

        /*if(level->fast_forward){
            build_timer.FastForward(level->number_of_updates_when_fast_forward);
            fall_timer.FastForward(level->number_of_updates_when_fast_forward);
        }*/

        PinguUpdate();

        if(is_rescued){
            level->rescued_pingus++;
            QueueForPurge();
        }

        //Draft for way of speeding up pingus when holding space
        //This requires accounting for spawners though which have a fixed time interval between spawns.
        if(level->fast_forward){
            for(int _ = 0; _ < level->number_of_updates_when_fast_forward; _++)
            {
                build_timer.FastForward(1.0f);
                fall_timer.FastForward(1.0f);
                driller_timer.FastForward(1.0f);
                wait_timer.FastForward(1.0f);
                PinguUpdate();
            }
        }

        //Antimatter pingus are black
        if(is_anti_matter){
            anim->current_animation_state->tint = olc::BLACK;
        }

    }

    void PinguCollision(){
        while(IsOverlappingSolid(local_position)){
            Console::PrintLine("PinguCollision: Unconventional conditions were met, resolving upwards before resuming with collision procedure");
            local_position.y -= 1.0f;
        }

        hit_floor_last_frame = hit_floor;

        //Resetting all collision related booleans
        hit_wall = false;
        hit_slope = true;
        hit_floor = false;
        hit_ceiling = false;

        //Currently unused
        bool attempt_free_from_semisolid = false;

        //Is the pingu already in semisolid?
        //To elaborate: the reason this is done, is to avoid pingus walking up a right-facing semisolid slope when coming to the left.

        /* This is ascii art of a pingu walking towards a semisolid slope from the opposite way of the way the pingu faces.
             ______
            /      _
           /      <*|
          /  <-- <|0|>
    _____/________- -_______

        */

        //When the pingu is very close to the slope, the following line of code detects if there is a piece of slope right above it's feet.
        //That way, it determines if its under that slope, which is likely. This can lead to a few oddities, but this will only happen with
        //very steep slopes.

        is_already_in_semi_solid = (IsOverlappingFeet(local_position+Vector2f(0.0f, -1.0f), olc::RED)
            || IsOverlappingFeet(local_position+Vector2f(0.0f, -2.0f), olc::RED));

        //This is the only time the pingu moves in the x-axis
        local_position.x += velocity.x * Engine::Get().GetDeltaTime();

        Vector2f local_position_before_resolving_wall = local_position;

        //Normal slope and walls
        if(IsOverlappingSolid(local_position)){
            bool slope_resolved = false;
            Vector2f incrementing_position = local_position;

            //Move back until one pixel before where Pingu collided
            while(IsOverlappingSolid(incrementing_position)){
                incrementing_position.x -= ufo::Maths::Sign(velocity.x);
            }

            while(!slope_resolved){

                //It is important to sore the position before the check that determines if the pixels in front of tux represent
                // a slope or a wall.
                Vector2f position_before_slope_incrementation = incrementing_position;

                //First the y position needs to be incremented repeatedly to measure how
                // many pixels a slope or potential wall is. It cannot exceed max_slope_height or
                // else it will be classified as a wall
                while(IsOverlappingSolid(incrementing_position)){

                    incrementing_position.y-=1.0f;

                    //If this if statement is true, then it's a wall.
                    if(std::abs(incrementing_position.y - position_before_slope_incrementation.y) > max_slope_height){

                        hit_slope = false;
                        hit_wall = true;
                        incrementing_position = position_before_slope_incrementation;
                        incrementing_position.x -= ufo::Maths::Sign(velocity.x);
                        slope_resolved = true;
                        break;

                    }
                }

                if(!hit_wall) incrementing_position.x += ufo::Maths::Sign(velocity.x);

                if(std::abs(local_position.x - incrementing_position.x) >= std::abs(velocity.x * Engine::Get().GetDeltaTime())){
                    slope_resolved = true;

                }
            }

            //No change in velocity.x if it's a slope, since you want to be able to walk up it properly
            if(hit_slope){
                local_position.y = incrementing_position.y;

            }
            if(hit_wall){

                local_position = incrementing_position;
                velocity.x = 0.0f;
            }

        }
        else{
            hit_slope = false;
        }

        if(IsOverlappingSolid(local_position)) Console::Print("Is still overlapping after resolution\n");

        //Semi solid slope

        //bool hit_semisolid_slope = false;

        //third boolean could be a parameter for this collision function perhaps
        if(IsOverlappingFeet(local_position, olc::RED) && (!is_already_in_semi_solid || what_is_current_state == States::BUILD || velocity.y > 0.0f /*|| hit_slope*/)){
            //hit_semisolid_slope = true;

            bool slope_resolved = false;

            Vector2f incrementing_position = local_position;

            Vector2f original_position_before_slope_check = local_position;

            while(IsOverlappingFeet(incrementing_position, olc::RED)){

                incrementing_position.y-=1.0f;

                if(IsOverlappingSolid(incrementing_position)){

                    incrementing_position.y+=1.0f;

                    face_direction *= -1.0f;
                    break;
                }

            }

            local_position = incrementing_position;
            velocity.y = 0.0f;

        }

        //The only modification of local_position.y internally. External modification from MovingSolid is still possible.
        local_position.y += velocity.y * Engine::Get().GetDeltaTime();

        //Collision-resolution in y-axis should there have been an overlap since the last movement
        if(IsOverlappingSolid(local_position)){

            //Move the pingu back until not colliding.
            while(IsOverlappingSolid(local_position)){
                local_position.y-=ufo::Maths::Sign(velocity.y);
            }

            //Setting hit_floor like this might not be necessary
            if(velocity.y > 0.0f) hit_floor = true;

            //This sort of ceiling detection might be unreliable since Pingu does not necessarily
            // collide each frame
            if(velocity.y < 0.0f) hit_ceiling = true;
            velocity.y = 0.0f;
        }

        //This should work each frame but somehow doesn't work on lower framerates
        if(IsOverlappingSolid(local_position+Vector2f(0.0f, 1.0f)) || IsOverlappingFeet(local_position+Vector2f(0.0f, 1.0f), olc::RED)){
            hit_floor = true;
        }

        //This is basically the snap-to-ground functionality
        if(!hit_floor && hit_floor_last_frame && !hit_slope && snap_to_ground_enabled){

            bool found_slope = true;
            Vector2f temporary_position = local_position;

            while(!IsOverlappingSolid(temporary_position) && !IsOverlapping(game, mask_decal, solid_layer, temporary_position, olc::RED)){
                temporary_position.y += 1.0f;
                if(std::abs(temporary_position.y - local_position.y) > max_slope_height*2.0f){

                    found_slope = false;
                    break;
                }
            }
            if(found_slope){
                hit_floor = true;
                local_position.y = temporary_position.y-1.0f;
                //velocity.y = 100.0f;
            }

        }

    }

    //olc::VERY_DARK_GREY and olc::WHITE count as normal solid
    bool IsOverlappingSolid(Vector2f _check_location){
        bool is_overlapping_moving_solid = false;

        for(auto&& moving_solid : level->moving_solids_handles){
            if(IsOverlappingMovingSolid(_check_location, olc::WHITE, moving_solid)) is_overlapping_moving_solid = true;
        }


        return (IsOverlapping(game, mask_decal, solid_layer, _check_location, olc::VERY_DARK_GREY) || IsOverlapping(game, mask_decal, solid_layer, _check_location, olc::WHITE)
            || is_overlapping_moving_solid);
    }

    void OnDraw(Camera* _camera){

        //Engine::Get().pixel_game_engine.DrawDecal(_camera->Transform(local_position),mask_decal);
        //DrawingSystem::DrawString({0.0f, 0.0f}, "Hello world", olc::WHITE, {1.0f,1.0f});
    }

    void OnWidgetDraw(){
        if(wait_timer.is_started){
            Vector2f count_text_screen_coordinate = level->GetActiveCamera()->Transform(GetGlobalPosition()) - Vector2f(3.0f,3.0f);
            Graphics::Get().DrawString(count_text_screen_coordinate, std::to_string(int(std::ceil(wait_timer.GetTimeLeft()/1000.0f))),Graphics::RED,Vector2f(1.0f,1.0f));
        }
    }

    //To detect if the furthest down row of pixles overlap with solid layer
    bool IsOverlappingFeet(Vector2f _position, olc::Pixel _colour){
        for(int i = 0; i < 12; i++){
            if(game->asset_manager.GetDecal(solid_layer)->sprite->GetPixel(_position.x+(float)i,_position.y + 23.0f) == _colour){
                return true;
            }
        }
        for(auto&& moving_solid : level->moving_solids_handles){
            if(IsOverlappingFeetMovingSolid(_position, _colour, moving_solid)) return true;
        }
        return false;
    }

    //To detect if the furtherest top row of pixles overlap with solid layer
    bool IsOverlappingHead(Vector2f _position, olc::Pixel _colour){
        for(int i = 0; i < 12; i++){
            if(game->asset_manager.GetDecal(solid_layer)->sprite->GetPixel(_position.x+(float)i,_position.y) == _colour){
                return true;
            }
        }
        for(auto&& moving_solid : level->moving_solids_handles){
            if(IsOverlappingHeadMovingSolid(_position, _colour, moving_solid)) return true;
        }
        return false;
    }

    bool IsOverlappingMovingSolid(Vector2f _position, olc::Pixel _colour, MovingSolid* _moving_solid){
        return (IsOverlappingOtherDecal(
            mask_decal, _position, level->asset_manager.GetDecal(_moving_solid->spr->key), _moving_solid->spr->GetGlobalPosition()
        ) || IsOverlappingOtherDecal(
            mask_decal, _position, level->asset_manager.GetDecal(_moving_solid->spr->key), _moving_solid->spr->GetGlobalPosition(), olc::VERY_DARK_GREY)
        );
    }

    //To detect if the furthest down row of pixles overlap with solid layer
    bool IsOverlappingFeetMovingSolid(Vector2f _position, olc::Pixel _colour, MovingSolid* _moving_solid){
        for(int i = 0; i < 12; i++){
            if(level->asset_manager.GetDecal(_moving_solid->spr->key)->sprite->GetPixel(_position.x+(float)i,_position.y + 23.0f) == _colour){
                return true;
            }
        }
        return false;
    }

    //To detect if the furtherest top row of pixles overlap with solid layer
    bool IsOverlappingHeadMovingSolid(Vector2f _position, olc::Pixel _colour, MovingSolid* _moving_solid){
        for(int i = 0; i < 12; i++){
            if(level->asset_manager.GetDecal(_moving_solid->spr->key)->sprite->GetPixel(_position.x+(float)i,_position.y) == _colour){
                return true;
            }
        }
        return false;
    }

    void OnMoved(MovingSolid* _moving_solid){
        if(what_is_current_state == States::BLOCKER){
            RemoveBlockerCollision(blocker_collision_position);
            ActivateBlockerCollision();
        }
    }

};
