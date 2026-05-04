
#include "Pipe.h"
#include "Constants.h"
#include <cstdlib>// dùng các hàm nh? rand()
#include <SDL.h>
#include <algorithm>

PipeManager::PipeManager() {
    Reset();
}

void PipeManager::CreatePipe() {
    int minY = 100;//?ây là gi?i h?n trên c?a t?a ?? Y cho ph?n ?ng trên (pipe top),Ngh?a là không cho phép ?ng xu?t hi?n sát mép trên màn hình (ch?a kho?ng 100px).
    int maxY = SCREEN_HEIGHT - PIPE_GAP - 100;//?ây là gi?i h?n d??i c?a t?a ?? Y, ?? ??m b?o ph?n d??i c?a ?ng trên cách ph?n d??i c?a màn hình ít nh?t PIPE_GAP + 100 px.
    int y = rand() % (maxY - minY + 1) + minY;//Rand() % (maxY - minY + 1) sinh s? ng?u nhiên trong ?o?n [0, maxY - minY]// khi tính ???c y s? là v? trí c?t xh




    pipes.push_back({ SCREEN_WIDTH, y, false }); //Thêm m?t ?ng m?i vào danh sách pipes,SCREEN_WIDTH: v? trí x b?t ??u c?a ?ng,y: v? trí y c?a ?ng, chim ch?a qua ?ng)
}

void PipeManager::Update() {
    Uint32 now = SDL_GetTicks();//Dòng này l?y th?i gian hi?n t?i (tính t? lúc SDL ???c kh?i ??ng) và l?u vào bi?n now. SDL_GetTicks() tr? v? th?i gian ?ã trôi qua t? khi game b?t ??u, tính b?ng mili giây.
    if (now - lastPipeTime > pipeInterval) {
        //N?u th?i gian trôi qua (now - lastPipeTime) l?n h?n pipeInterval, ngh?a là ?ã ??n lúc t?o c?t m?i.
        CreatePipe();
        lastPipeTime = now;
        //Sau khi t?o c?t m?i, lastPipeTime s? ???c c?p nh?t l?i b?ng th?i gian hi?n t?i (now), ?? tính cho l?n t?o c?t ti?p theo
    }

    for (auto& pipe : pipes) {//duy?t qua danh sách c?t
        pipe.x -= speed;//M?i c?t trong pipes có m?t thu?c tính x, bi?u th? v? trí c?a nó trên tr?c hoành (trái ph?i),có t?c ??
        if (!pipe.passed && pipe.x + PIPE_WIDTH < 200) {//ki?m tra xem c?t có ?i qua m?t v? trí xác ??nh trên màn hình (? ?ây là x = 200), có ngh?a là chim ?ã v??t qua c?t này.
            pipe.passed = true;
            score++;//Khi chim v??t qua c?t, ?i?m s? (score) s? ???c t?ng lên m?t ??n v?.
            if (score > highScore) highScore = score;//Sau khi t?ng ?i?m, ta ki?m tra xem ?i?m hi?n t?i (score) có l?n h?n ?i?m cao nh?t tr??c ?ó (highScore) hay không
        }
    }

    if (!pipes.empty() && pipes.front().x + PIPE_WIDTH < 0) {
        //pipes.front() l?y ph?n t? ??u tiên trong danh sách c?t. N?u ph?n t? này ?ã ?i ra ngoài màn hình (t?c là pipe.x + PIPE_WIDTH < 0
        // , có ngh?a là c?t ?ã r?i kh?i màn hình bên trái), thì ta c?n xóa nó ?i.
        pipes.erase(pipes.begin());
    }
}

void PipeManager::Render(SDL_Renderer* renderer, SDL_Texture* texture) {//v? c?t
    for (auto& pipe : pipes) {
        
        SDL_Rect top = { pipe.x, 0, PIPE_WIDTH, pipe.y };//t?o m?t SDL_Rect ?? ??i di?n cho ph?n "??u" c?a c?t.(vtri ngang c?a c?t,
        SDL_RenderCopy(renderer, texture, nullptr, &top);//Dùng hàm SDL_RenderCopy ?? v? ph?n "??u" c?a c?t.(v?,?nh c?t,l?y h?t toàn b? ?nh)
        SDL_Rect bottom = {
            pipe.x,
            pipe.y + PIPE_GAP,
            PIPE_WIDTH,
            SCREEN_HEIGHT - (pipe.y + PIPE_GAP)
        };
        SDL_RenderCopyEx(renderer, texture, nullptr, &bottom, 180, nullptr, SDL_FLIP_NONE);
    }
}

bool PipeManager::CheckCollision(const SDL_Rect& bird) {// va ch?m
    for (const auto& pipe : pipes) {
        SDL_Rect topRect = { pipe.x, 0, PIPE_WIDTH, pipe.y };
        //T?o m?t SDL_Rect ??i di?n cho ph?n "??u" c?a c?t (ph?n không có kho?ng tr?ng), t? v? trí pipe.x và có chi?u cao b?ng pipe.y
        SDL_Rect bottomRect = { pipe.x, pipe.y + PIPE_GAP, PIPE_WIDTH, SCREEN_HEIGHT - pipe.y - PIPE_GAP };
        //T?o m?t SDL_Rect ??i di?n cho ph?n "d??i" c?a c?t (ph?n sau kho?ng tr?ng), t? v? trí pipe.x và b?t ??u t? v? trí pipe.y + PIPE_GAP (kho?ng cách gi?a hai c?t).
        if (SDL_HasIntersection(&bird, &topRect) || SDL_HasIntersection(&bird, &bottomRect)) {
            //Hàm SDL_HasIntersection ki?m tra xem có s? giao nhau (va ch?m) gi?a hình ch? nh?t c?a chim (bird) và ph?n "??u" (topRect) ho?c ph?n "d??i" (bottomRect) c?a c?t không.
            return true;
        }
    }
    return false;
}

void PipeManager::Reset() {
    pipes.clear();
    score = 0;
    lastPipeTime = SDL_GetTicks();
}

int PipeManager::GetScore() {
    return score;
}

int PipeManager::GetHighScore() {
    return highScore;
}
