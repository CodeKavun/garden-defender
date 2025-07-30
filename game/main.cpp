#include <iostream>
#include "engine/core.hpp"

int main() {
    Game game("Garden Defender", 1024, 768);
    game.run();
    return 0;
}