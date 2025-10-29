#pragma once
#include <thread>
#include <functional>

namespace UFOEngineStudio{

class ProgramState;

class GameEngineThread{
public:
    std::function<void(ProgramState*)> process = [](ProgramState* _){};

};

void GameEngineThreadRun(ProgramState* _program);

}