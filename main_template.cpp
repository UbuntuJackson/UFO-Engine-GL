#include <memory>
#include <Main.h>
#include "src/my_level/my_level.h"

int main(){
    auto main = ufo::Main();

    auto e = std::make_unique<ufo::Engine>(800, 600);

    e->level = std::move(std::make_unique<MyLevel>());

    main.Start(std::move(e));

    return 0x0;
}