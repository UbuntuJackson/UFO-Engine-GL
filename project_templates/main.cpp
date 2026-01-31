#include <engine.h>
#include <exception>
#include <memory>
#include <level.h>
#include <console.h>
#include <stdexcept>
#include "generated.h"
#include <level_loader.h>

int main(){

    auto engine = std::make_unique<ufo::Engine>();
    engine->InitIndependant();
    engine->actor_generator = std::make_unique<Generated::ActorGenerator>();
    engine->actor_generator->Initialise();
    try{
        engine->GoToLevel("../path_to_my_level.ason");

        engine->Start();
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    return 0x0;

}
