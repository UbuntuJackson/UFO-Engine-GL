#include <exception>
#include <memory>
#include <editor.h>
#include <console.h>
#include <level.h>
#include <actor.h>
#include <engine.h>
#include <stdexcept>

int main(){

    //Console::EnableLoggingFile();

    auto e = std::make_unique<ufo::Engine>();

    e->InitIndependant();
    e->loaded_levels.push_back(std::make_unique<ufo::Level>());
    e->pending_levels.push_back(e->loaded_levels.back().get());
    e->StartWithImGui();

    //Console::CloseLoggingFile();

    //Console::Input("The program has exited. Press any key to close the terminal...");

    return 0x0;
}
