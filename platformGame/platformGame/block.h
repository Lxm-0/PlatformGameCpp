#pragma once
#include <SDL.h>
#include <iostream>
using namespace std;

class Block {
public:
    SDL_Rect rect;
    SDL_Texture* texture;
    int type;
    bool active;

    Block(SDL_Texture* tex, int x, int y, int w, int h, int t) {
        texture = tex;
        rect = { x, y, w, h };
        type = t;
        active = true;
    }

    void draw(SDL_Renderer* renderer) {
        if (active) {
            SDL_RenderCopy(renderer, texture, NULL, &rect);
        }
    }
};
