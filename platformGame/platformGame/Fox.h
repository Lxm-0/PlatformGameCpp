#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include "map.h"
#include "block.h"
#include <SDL_mixer.h>

using namespace std;




class  Fox  {
public:
    SDL_Rect rect;
    SDL_Texture* texture;
    float velY = 0;
    float velX = 0;
    bool onGround = false;
    bool gravity_active = true;
    int playerScore = 0;
    int jumpCount = 0;
    bool isDead = false;
    bool keystatSound = false;
    Mix_Chunk* coins;

    Fox(SDL_Renderer* renderer, const char* path, int x, int y, int w, int h,
        Mix_Chunk* coinsSound)
        : coins(coinsSound)
    {
        SDL_Surface* surface = IMG_Load(path);
        if (!surface) cerr << "IMG_Load Error: " << IMG_GetError() << '\n';
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        rect = { x, y, w, h };
    }

    void handleInput(const Uint8* keystate) {
        if (isDead)return;
        keystatSound = false;

        velX = 0;
        if (keystate[SDL_SCANCODE_RIGHT]) { velX = 4; keystatSound = true; }
        if (keystate[SDL_SCANCODE_LEFT]){ velX = -4; keystatSound = true;
    }

        if ((keystate[SDL_SCANCODE_SPACE] || keystate[SDL_SCANCODE_UP]) && onGround) {
            velY = -8;
            jumpCount++;
            onGround = false;
            gravity_active = true;
            keystatSound = true;
        }
    }

    void update(vector<Block>& blocks) {

        rect.x += velX;


        for (auto& b : blocks) {
            if (!b.active) continue;
            if (!SDL_HasIntersection(&rect, &b.rect)) continue;

            if (b.type == 3) {
                isDead = true;
                velX = velY = 0;
                gravity_active = false;
                onGround = false;
                return;
            }

            if (b.type == 4) continue;

            if (velX > 0) {
                rect.x = b.rect.x - rect.w;

            }
            else if (velX < 0) rect.x = b.rect.x + b.rect.w;
            velX = 0;

        }

        if (gravity_active) {
            velY += 0.5f;
            if (velY > 10) velY = 10;
        }
        rect.y += velY;
        onGround = false;

        for (auto& b : blocks) {
            if (!b.active) continue;
            if (!SDL_HasIntersection(&rect, &b.rect)) continue;

            if (b.type == 3) {                     // بلوك قاتل
                isDead = true;
                velX = velY = 0;
                gravity_active = false;
                onGround = false;
                return;
            }

            if (b.type == 4) continue;

            if (velY > 0) {
                rect.y = b.rect.y - rect.h;
                velY = 0;
                onGround = true;
                jumpCount = 0;
            }
            else if (velY < 0) {
                rect.y = b.rect.y + b.rect.h;
                velY = 0;
            }
        }
        for (auto& b : blocks) {
            if (b.type == 4 && b.active && SDL_HasIntersection(&rect, &b.rect)) {
                playerScore++;
                b.active = false;
                Mix_PlayChannel(-1,coins, 0);
            }
        }


    }



    void draw(SDL_Renderer* renderer) {
        SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL;

        if (velX < 0) {//حركه حلوه عشان اخفى الاعب بالعابي المستقبليه

            flip = SDL_FLIP_NONE;
        }
        else if (velX > 0) {
            flip = SDL_FLIP_HORIZONTAL;
        }
        SDL_RenderCopyEx(renderer, texture, NULL, &rect, 0, NULL, flip
        );
    }

    ~Fox() {
        SDL_DestroyTexture(texture);
    }
};
