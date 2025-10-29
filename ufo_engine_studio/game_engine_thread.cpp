#include "game_engine_thread.h"
#include "program_state.h"
#include "../console/console.h"

namespace UFOEngineStudio{

void GameEngineThreadRun(ProgramState* _program){
    while(true){
        //Console::PrintLine("GameEngineThread");
        _program->game_engine_thread.process(_program);
        _program->game_engine_thread.process = [](ProgramState* _){};
    }
}

}