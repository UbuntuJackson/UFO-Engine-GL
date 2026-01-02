#include <Main.h>
#include <engine.h>
#include <exception>
#include <memory>
#include <level.h>
#include <console.h>
#include <stdexcept>
#include "generated.h"
#include "UFO-Engine/ufo_garbage_collector/gc_json.h"
#include "UFO-Engine/ufo_garbage_collector/garbage_collector.h"

class LevelLoader : public ufo::gc::Root{
public:
    LevelLoader() = default;
     std::unique_ptr<Actor> LoadLevel(ufo::Engine* _engine, const std::string& _level){
        auto level_json = ufo::gc::JsonRead(&gc, _level);
        if(level_json->IsNull()){
            throw std::runtime_error("[UFO-Engine] LevelLoader::LoadLevel: Could not load level "+ _level+".");
        }

        auto level = _engine->actor_generator->JsonToActorTree(&gc,level_json);
        return std::move(level);
     }
};

int main(){

    auto main = ufo::Main(1600,900,"Blit Bloot");

    auto engine = std::make_unique<ufo::Engine>();
    //engine->level = std::make_unique<MyLevel>();
    engine->actor_generator = std::make_unique<Generated::ActorGenerator>();
    engine->actor_generator->Initialise();
    try{
        engine->level = LevelLoader().LoadLevel(engine.get(), "../test_level.ason");
        engine->level_handle = engine->level->DynamicCast<Level>();

        main.Start(std::move(engine));
    } catch(const std::exception& _error){
        Console::PrintLine(_error.what());
    }

    return 0x0;

}
