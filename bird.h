#pragma once
#include <SDL.h>

class Bird {
public:
    SDL_Rect rect;
    float velocity;

    Bird();
    void Update();
    void Jump();
    void Reset();
};