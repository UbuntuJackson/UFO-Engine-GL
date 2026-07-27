#include <memory>
#include <engine.h>
#include <exception>
#include <level.h>
#include <console.h>
#include <stdexcept>
#include "generated.h"
#include <ufo_macros.h>
#include <level_loader.h>
#include <file_utils.h>
#include <shared_json.h>

int main(int _arg_count, char** _arg_variables){

    std::string start_scene = "";

    if(ufo::FileSystem::FileExists("../settings.json")){
        auto shared_json_settings = ufo::SharedMemory::JsonRead("../settings.json");
        if(shared_json_settings->map.count("start_scene")){
            shared_json_settings->TryToGetValueAsString("start_scene", &start_scene, __UFO_PRETTY_FUNCTION__+" Error, could not find start_scene");
        }
    }

    if(start_scene == "" && _arg_count == 1){
        start_scene = *_arg_variables;
    }

    if(start_scene == ""){
        Console::PrintLine("Error, could not set a start scene");
        return 0xBAD;
    }

    ufo::Engine engine;
    engine.InitIndependant();
    engine.actor_generator = std::make_unique<ufo::Generated::ActorGenerator>();
    engine.actor_generator->Initialise();
    try{
        engine.GoToLevel(start_scene);
        engine.Start();
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    return 0x0;

}
