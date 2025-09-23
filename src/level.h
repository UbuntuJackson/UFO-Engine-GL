#pragma once

#include <vector>
#include <memory>
#include "actor.h"

class Camera;

namespace ufo{
    class Controls;
    class Graphics;
}

class Level : public Actor{
public:
    Vector2f size = {2000.0f, 1000.0f};

    enum CleanUpModes{
        EACH_FRAME,
        CYCLICAL
    };

    std::vector<int> deletion_history;

    std::vector<Camera*> camera_handles;
    std::vector<Camera*> active_camera_handles;

    int clean_up_mode = CleanUpModes::EACH_FRAME;

    Level();

    int control_handout_counter = -1;
    ufo::Controls* GetControls();

    //If you want the bare minimum for a functioning level
    virtual void LoadBlank(){

    }

    virtual void Load();

    void OnAddActor(Actor* _actor){
        _actor->level = this;
    }

    void LoadEngineObjects(/*JsonUnion _level_file*/){/**/}

    virtual void LoadCustomObjects(/*JsonUnion _level_file*/){
        //Generated::AssembleCustomActors(_level_file);
    }

    virtual ~Level(){}

    void UpdatePhrase(float _delta_time);

    void DrawPhase(ufo::Graphics* _graphics);

};