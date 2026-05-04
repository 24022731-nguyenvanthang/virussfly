#ifndef PIPE_H
#define PIPE_H

#include <vector>
#include <SDL.h>

struct Pipe {
    int x;
    int y;
    bool passed;
};

class PipeManager {
public:
    PipeManager();

    void CreatePipe();
    void Update();
    void Render(SDL_Renderer* renderer, SDL_Texture* texture);
    bool CheckCollision(const SDL_Rect& bird);
    void Reset();
    int GetScore();
    int GetHighScore();

    int speed = 2;

private:
    std::vector<Pipe> pipes;
    int score = 0;
    int highScore = 0;
    Uint32 lastPipeTime = 0;
    int pipeInterval = 2000;// kho?ng th?i gian gi?a 2 c?t ???c t?o ra
};

#endif // PIPE_H
