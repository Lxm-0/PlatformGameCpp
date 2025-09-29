#pragma once
#include "Map.h"
#include "block.h"
#include "Fox.h"
#include "Points.h"
#include <fstream>
#include <SDL_mixer.h>
using namespace std;

class Game {
public:
    SDL_Window* window;
    SDL_Renderer* renderer;
    bool running;
    Fox* fox;
    vector<Block> blocks;
    TTF_Font* font;
    POoints Points;
    SDL_Texture* blockTexture1;
    SDL_Texture* gameOverTexture;
    SDL_Texture* blockTexture2;
    SDL_Texture* spikeTexture;
    SDL_Texture* pointTexture;
    Mix_Music* GameOverSound;
    Mix_Music* walking = nullptr;
    Mix_Chunk* coins = nullptr;


    enum GameState {
        PLAYING, GAMEOVER
    };
    GameState state = PLAYING;
    int foxStartX = 10;
    int foxStartY = 500 - 300;
    void resetGame() {
        fox->rect.x = foxStartX;
        fox->rect.y = foxStartY;
        fox->velX = 0;
        fox->velY = 0;
        fox->onGround = false;
        fox->gravity_active = true;
        fox->jumpCount = 0;
        fox->isDead = false;
        state = PLAYING;
        for (auto& b : blocks) {
            if (b.type == 4) {
                b.active = true;
            }
        }
    }

    Game() : window(nullptr), renderer(nullptr), running(false), fox(nullptr),
        blockTexture1(nullptr), blockTexture2(nullptr), spikeTexture(nullptr), GameOverSound(nullptr) {}



    bool init() {

        if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
        if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;
        if (TTF_Init() == -1) {
            SDL_Log("Unable to initialize SDL_ttf: %s", TTF_GetError());
        }

        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            printf("SDL_mixer could not initialize! %s\n", Mix_GetError());
        }




        if (!GameOverSound) {
            printf("Failed to load game over music: %s\n", Mix_GetError());
        }


        window = SDL_CreateWindow("Fox Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, 0);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        font = TTF_OpenFont(R"(C:\\Windows\\Fonts\arial.ttf)", 20);
        blockTexture1 = IMG_LoadTexture(renderer, R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\images\stripped_cherry_log.png)");
        blockTexture2 = IMG_LoadTexture(renderer, R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\images\pink_petals.png)");
        spikeTexture = IMG_LoadTexture(renderer, R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\images\tube_coral_fan.png)");
        pointTexture = IMG_LoadTexture(renderer, R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\images\snowball.png)");

        //الصوت حق الفلوس والمشي واللعبه لما تكمل
        GameOverSound = Mix_LoadMUS(R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\sounds\GameOver.wav)");
        walking = Mix_LoadMUS(R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\sounds\Walking.wav)");
        coins = Mix_LoadWAV(R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\sounds\coin.wav)");

        fox = new Fox(renderer,
            R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\images\snow_fox_sleep.png)",
            foxStartX, foxStartY,
            24,
            24, coins);

        Points.load();
        fox->playerScore = Points.score;

        // إنشاء البلوكات من المصفوفة
        for (int y = 0; y < MAP_HEIGHT; y++) {
            for (int x = 0; x < MAP_WIDTH; x++) {
                int type = map[y][x];
                SDL_Texture* tex = nullptr;
                if (type == 1) tex = blockTexture1;
                else if (type == 2) tex = blockTexture2;
                else if (type == 3) tex = spikeTexture;
                else if (type == 4) tex = pointTexture;

                if (tex) blocks.emplace_back(tex, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, type);

            }
        }

        running = true;
        return true;
    }

    void loop() {
        gameOverTexture = IMG_LoadTexture(renderer, R"(C:\Users\malak abd alqwy\source\repos\platformGame\x64\Debug\images\GameOver.png)");
        if (!gameOverTexture) {
            cerr << "Failed to load GameOver image: " << IMG_GetError() << endl;
        }

        while (running) {
            SDL_Event event;

            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) running = false;

                if (state == GAMEOVER && event.type == SDL_KEYDOWN) {

                    if (event.key.keysym.sym == SDLK_r) {
                        resetGame();
                    }
                    else if (event.key.keysym.sym == SDLK_q) {
                        running = false;
                    }
                }

            }

            if (state == PLAYING) {

                const Uint8* keystate = SDL_GetKeyboardState(NULL);
                fox->handleInput(keystate);
                fox->update(blocks);

                bool shouldPlayFootstep = fox->keystatSound && fox->onGround;

                if (shouldPlayFootstep) {
                            Mix_PlayMusic(walking, 0);
                }
                else {
                  !fox->keystatSound;
                }

                if (fox->playerScore != Points.score) {
                    Points.score = fox->playerScore;
                    Points.save();
                }

                if (fox->isDead) {
                    state = GAMEOVER;

                    if (GameOverSound) {
                        Mix_HaltMusic();
                        Mix_PlayMusic(GameOverSound, 0);
                    }
                }

            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);


            for (auto& b : blocks) {
                if (b.type != 4 || (b.type == 4 && b.active)) {
                    b.draw(renderer);
                }

            }
            fox->draw(renderer);

            string scoreText = "Score: " + to_string(Points.score);
            SDL_Color black = { 0,0,0,255 };
            TTF_SetFontStyle(font, TTF_STYLE_BOLD);
            SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, scoreText.c_str(), black);
            SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);

            SDL_Rect messageRect;
            messageRect.x = 30;
            messageRect.y = 3;
            messageRect.w = surfaceMessage->w;
            messageRect.h = surfaceMessage->h;

            SDL_RenderCopy(renderer, message, NULL, &messageRect);

            SDL_FreeSurface(surfaceMessage);
            SDL_DestroyTexture(message);



            // لو خسرنا نعرض هذي الصورة
            if (state == GAMEOVER && gameOverTexture) {
                SDL_Rect dst = { 300, 200, 600, 200 };
                SDL_RenderCopy(renderer, gameOverTexture, NULL, &dst);

            }

            SDL_RenderPresent(renderer);
            SDL_Delay(16); 
        }
    }

    void clean() {
        delete fox;
        SDL_DestroyTexture(blockTexture1);
        SDL_DestroyTexture(blockTexture2);
        SDL_DestroyTexture(spikeTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        TTF_CloseFont(font);
        TTF_Quit();
        Points.save();
        Mix_FreeChunk(coins);
        Mix_FreeMusic(walking );
        Mix_FreeMusic(GameOverSound);
    }
};