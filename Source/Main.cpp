#include "Game.h"
#include <memory>

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    auto game = std::make_unique<Game>();

    if (!game->IsInitialized()) {
        return -1;
    }

    game->Run();
    return 0;
}