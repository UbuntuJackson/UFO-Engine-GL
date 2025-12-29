#pragma once
#include "../ufo_garbage_collector/engine_memory.h"

class OpenGLv4_5_AssetManager;

class AssetJson : public ufo::gc::Root{
public:
    void Read(const std::string& _path, OpenGLv4_5_AssetManager* _asset_manager);

    void Write(const std::string& _path, OpenGLv4_5_AssetManager* _asset_manager);

};
