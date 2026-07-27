#include <memory>
#include <stdexcept>
#include "ufo_engine_studio/editor.h"
#include "utils/console.h"
#include "src/actor.h"

int main(){

    //Console::EnableLoggingFile();
try{
    auto e = std::make_unique<ufo::Engine>();

    e->InitIndependant();
    e->loaded_levels.push_back(std::make_unique<UFOEngineStudio::Editor>());
    e->pending_levels.push_back(e->loaded_levels.back().get());
    e->StartWithImGui();
}catch(const std::runtime_error& _error){
    Console::PrintLine("Unhandled exception", _error.what());
}
catch(...){
    Console::PrintLine("Unhandled exception");
}
    //Console::CloseLoggingFile();

    //Console::Input("The program has exited. Press any key to close the terminal...");

    return 0x0;
}
