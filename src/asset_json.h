#pragma once
#include "../ufo_garbage_collector/object.h"

namespace ufo{
class OpenGLv4_5_AssetManager;
class Engine;

class AssetJson : public ufo::gc::Root{
public:
    void Read(const std::string& _path, const std::string& _opened_directory_path, ufo::OpenGLv4_5_AssetManager* _asset_manager, ufo::Engine* _engine);

    void Write(ufo::OpenGLv4_5_AssetManager* _asset_manager);

};
}
