#pragma once

#include <vector>
#include <memory>
#include "actor.h"
#include "../tilemap/tileset_manager.h"

class Camera;

namespace ufo{
    class Controls;
    class Graphics;
}

class Level : public Actor{
public:
    TilesetManager tileset_manager;

    Vector2f size = {20000.0f, 3000.0f};

    enum CleanUpModes{
        EACH_FRAME,
        CYCLICAL
    };

    std::vector<int> deletion_history;

    std::vector<Camera*> camera_handles;
    std::vector<Camera*> active_camera_handles;

    int clean_up_mode = CleanUpModes::EACH_FRAME;

    Level();
    void OnSpawn();

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

    void Update(float _delta_time);

    void UpdatePhrase(float _delta_time);

    void DrawPhase(ufo::Graphics* _graphics);

    virtual void EditorUpdatePhase(float _delta_time){

    }

    virtual void EditorDrawPhase(){

    }

};
