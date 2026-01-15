#pragma once

#include "../src/texture_2d.h"
#include <cstdint>

namespace ufo::stack{

const std::uint8_t max_number_of_assets = 50;

class AssetManager{
public:

    std::uint8_t number_of_assets = 0;

    ufo::Texture2D textures[max_number_of_assets];

    AssetManager(){

    }

    std::uint8_t AddTexture(const std::string _texture_path){
        textures[++number_of_assets] = ufo::Texture2D();

        return number_of_assets;
    }

};

}
