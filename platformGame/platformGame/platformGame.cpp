#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <iostream>
#include <string>

#include "Map.h"
#include "block.h"
#include "Fox.h"
#include "Game.h"
#include "Points.h"


using namespace std;

int main(int argc, char* argv[]) {
    Game game;
    if (!game.init()) return -1;
    game.loop();
    game.clean();
    return 0;
}