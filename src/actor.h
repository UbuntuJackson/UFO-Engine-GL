#pragma once
#include <memory>
#include <vector>
#include "../ufo_maths/ufo_maths.h"
#include "graphics.h"

namespace ufo{
    class Engine;
}

class Level;

//Using rule of 0 3 and 5 for all actors
class Actor{
public:

    Vector2f local_position;

    bool is_dead = false;

    Actor(Vector2f _local_position);
private:
    Actor* parent = nullptr;
public:
    ufo::Engine* engine = nullptr;
    Level* level = nullptr;

    Vector2f GetGlobalPosition();

    //Reserve a set number of actors?
    std::vector<std::unique_ptr<Actor>> actors;

    std::vector<std::unique_ptr<Actor>> new_actor_queue;

    void AddNewActors(){
        for(auto&& actor : new_actor_queue){
            actor->AddNewActors();
            actors.push_back(std::move(actor));
        }
        new_actor_queue.clear();
    }

    template <typename tActor, typename ...tArgs>
    tActor* AddActor(tArgs&& ..._args){
        auto u_actor = std::make_unique<tActor>(_args...);
        auto actor_ptr = u_actor.get();
        actor_ptr->parent = this;
        actor_ptr->level = level;
        OnAddActor(actor_ptr);
        actor_ptr->engine = engine;
        actor_ptr->OnSpawn();
        new_actor_queue.push_back(std::move(u_actor));
        return actor_ptr;
    }

    //Actor(const Actor&) = delete;
    //Actor(Actor&&) = delete;

    void CleanUpDeadActors();

    void MarkAllDead();

    virtual void OnSpawn();

    virtual void OnAddActor(Actor* _actor);

    void Update(float _delta_time);

    virtual void OnUpdate(float _delta_time){

    }

    void WidgetDraw(ufo::Graphics* _graphics){
        OnWidgetDraw(_graphics);
        for(const auto& actor : actors){
            actor->WidgetDraw(_graphics);
        }
    }

    virtual void OnWidgetDraw(ufo::Graphics* _graphics){
        
    }

    void Draw(ufo::Graphics* _graphics, Camera* _camera){
        OnDraw(_graphics, _camera);
        for(const auto& actor : actors){
            actor->Draw(_graphics, _camera);
        }
    }

    virtual void OnDraw(ufo::Graphics* _graphics, Camera* _camera){

    }

    template<typename tActor>
    tActor* DynamicCast(){
        return dynamic_cast<tActor*>(this);
    }

    template<typename tActor>
    tActor* StaticCast(){
        return static_cast<tActor*>(this);
    }

    virtual ~Actor(){
        
    }
};