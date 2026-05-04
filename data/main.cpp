#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Bird.h"
#include "Pipe.h"
#include "Constants.h"

#undef main

SDL_Window* g_window = nullptr;//khai báo một con trỏ tới kiểu dữ liệu SDL_Window và khởi tạo nó bằng giá trị nullptr (rỗng).|| lưu địa chỉ trỏ tới nó
SDL_Renderer* g_renderer = nullptr;// giống trên và để vẽ mọi thứ lên cửa sổ win dow/g_renderer là biến toàn cục để truy cập vào
SDL_Texture* birdTexture = nullptr;//gắn hình ảnh lên cửa sổ thông qua renderer
SDL_Texture* backgroundTexture = nullptr;//dùng để chứa và vẽ ảnh nền của game trong mỗi khung hình.
SDL_Texture* pipeTexture = nullptr;//Khai báo con trỏ đến SDL_Texture dùng để chứa ảnh của ống nước (pipe)
SDL_Texture* rankingBackground = nullptr;//kiểu con trỏ tró tới texture đã được lưu trong cpu
TTF_Font* g_font = nullptr; //một con trỏ đến một font chữ(kiểu TTF_Font*) dùng trong SDL_ttf,g_font là tên biến lưu thông tin về font chữ

Mix_Music* backgroundMusic = nullptr;//mix music đại điện cho mp3
Mix_Chunk* jumpSound = nullptr;//Mix_Chunk đại diện cho một đoạn âm thanh ngắn
Mix_Chunk* collisionSound = nullptr;//Mix_Chunk dùng để lưu một đoạn âm thanh ngắn — như tiếng va chạm, tiếng bắn, tiếng click, v.v.

Bird bird;// class chim
PipeManager pipeManager;//class cột 

bool gameMenu = true; //Gán giá trị ban đầu là true ⇒ nghĩa là khi game mới khởi động lên, nó đang ở menu.
bool gameOver = false;//Giá trị ban đầu là false, vì lúc mới vào game chưa thua.
bool showRanking = false; //giá trị khởi đầu là false ⇒ ban đầu không hiện bảng xếp hạng.

std::vector<int> highScores;// mảng đếm điểm

bool init() {//khởi tạo toàn bộ hệ thống SDL, bao gồm: video, âm thanh, font chữ, cửa sổ, renderer… để chuẩn bị cho game hoạt động.
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) return false;//khởi tạo cả hệ thống video (màn hình, cửa sổ) và audio, nếu < 0 sẽ tắt
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;//tạo hệ thống âm thanh của sdl mixer,44100 là tần số,định dạng,số kênh,bộ nhớ đệm
    if (TTF_Init() == -1) return false;// sử lí font chữ nếu lỗi trả về false

    g_window = SDL_CreateWindow("Flappy Bird", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!g_window) return false;// tạo cửa sổ wdow tên flappy bớt,đặt vào giữa màn hình,chiêu rộng,ccao,hiển thị cửa sổ ngay khi tạo

    g_renderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);// tạo 1 renderer vẽ để liên kết cửa sổ flappy bớt,-1 là dùng driver mặc định,
    //SDL_RENDERER_ACCELERATED: cho phép sử dụng tăng tốc phần cứng
    if (!g_renderer) return false;

    g_font = TTF_OpenFont("Cleanow.ttf", 36);//Mở font chữ Cleanow.ttf với cỡ chữ 36 pixel.
    return g_font != nullptr;
}

bool loadMedia() {
    SDL_Surface* surface = IMG_Load("meo.png");//Tải ảnh nền từ file "meo.png" và lưu vào surface tạm thời (kiểu SDL_Surface)
    if (!surface) return false;
    backgroundTexture = SDL_CreateTextureFromSurface(g_renderer, surface);//Tạo một SDL_Texture từ SDL_Surface đã tải ảnh,đưa ra màn hình,g_renderer là SDL_Renderer xử lí vẽ ảnh ra mh
   //surface là SDL_Surface chứa dữ liệu ảnh đã load từ file (ví dụ "meo.png").
    SDL_FreeSurface(surface);//Giải phóng bộ nhớ của SDL_Surface sau khi đã chuyển sang SDL_Texture. tránh rò rỉ


    surface = IMG_Load("chim.png");//để tải hình ảnh từ file vào chương trình,con trỏ surface chứa dữ liệu hình ảnh sau khi load
    if (!surface) return false;
    birdTexture = SDL_CreateTextureFromSurface(g_renderer, surface);//g_renderer là con trỏ dùng để vẽ lên cửa sổ,
    //SDL_CreateTextureFromSurface(...): Hàm này chuyển dữ liệu ảnh từ dạng SDL_Surface sang SDL_Texture vì tối ưu hơn cho phần cứng
    SDL_FreeSurface(surface);// giải phóng

    surface = IMG_Load("pipe.png");//img_load tải ảnh từ pipe và lưu vào surface tạm thời
    if (!surface) return false;
    pipeTexture = SDL_CreateTextureFromSurface(g_renderer, surface);// tương tự
    SDL_FreeSurface(surface);

    surface = IMG_Load("ranking_bg.png");
    if (!surface) return false;
    rankingBackground = SDL_CreateTextureFromSurface(g_renderer, surface);// tương tự
    SDL_FreeSurface(surface);

    backgroundMusic = Mix_LoadMUS("bgr.mp3");//là hàm dùng để load nhạc định dạng dài, sử dụng nhạc ngay trong background
    jumpSound = Mix_LoadWAV("jump.wav");//dùng để load âm thanh dạng WAV, phát âm thanh khi nhảy
    collisionSound = Mix_LoadWAV("die.wav");// tương tự phát âm thanh khi chết

    return backgroundMusic && jumpSound && collisionSound;
}

void close() {
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(birdTexture);
    SDL_DestroyTexture(pipeTexture);
    SDL_DestroyTexture(rankingBackground);
    TTF_CloseFont(g_font);

    Mix_FreeChunk(jumpSound);
    Mix_FreeChunk(collisionSound);
    Mix_FreeMusic(backgroundMusic);
    Mix_CloseAudio();

    SDL_DestroyRenderer(g_renderer);
    SDL_DestroyWindow(g_window);

    TTF_Quit();
    SDL_Quit();
} // giải phóng tài nguyên
// hàm vẽ văn bản lên màn hình
void drawText(const std::string& text, int x, int y, SDL_Color color) {//(chuỗi cần vẽ,toạ độ nơi vẽ,màu của văn bản.
    SDL_Surface* surface = TTF_RenderText_Solid(g_font, text.c_str(), color);//tạo ra một SDL_Surface (một hình ảnh tạm thời),text.c chuyển thành kiểu char
    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_renderer, surface);//giúp chuyển một SDL_Surface (ảnh tạm chứa văn bản hoặc hình ảnh) thành một SDL_Texture để có thể vẽ lên màn hình bằng SDL_Renderer.
    SDL_Rect rect = { x, y, surface->w, surface->h };//vị trí và kích thước của đoạn văn bản vẽ lên màn hình,surface->w và surface->h: Là chiều rộng và chiều cao của ảnh/văn bản bạn vừa tạo bằng TTF_RenderText_Solid.
    SDL_RenderCopy(g_renderer, texture, nullptr, &rect);// vẽ 1 ảnh lên cửa sổ game(nơi vẽ,ảnh chứa chữ từ suface trước,lấy full ảnh,vị trí ảnh
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);// giải phóng
}

void drawScore() {// vẽ điểm hiện tại và cao nhất
    SDL_Color color = { 255, 255, 255 };// Khởi tạo một biến kiểu SDL_Color (cấu trúc màu RGB), giá trị này là màu trắng (255, 255, 255).
    std::string scoreText = "Score: " + std::to_string(pipeManager.GetScore()) + "  High: " + std::to_string(pipeManager.GetHighScore());
    //khai báo score text Biến này sẽ chứa dòng chữ để hiển thị điểm số, đoạn sau là lấy điểm số từ pipeman với getscore rồi chuyển thành chuỗi string
    drawText(scoreText, 10, 10, color);//sẽ hiển thị dòng chữ scoreText lên màn hình tại vị trí (10, 10) với màu trắng
}

void drawMenu() {// vẽ menu chính
    SDL_RenderCopy(g_renderer, backgroundTexture, nullptr, nullptr);// vẽ nền menu,nullptr,texture là meo.png nullptr: vùng nguồn và vùng đích để vẽ toàn bộ ảnh nền full screen

    SDL_Color white = { 255, 255, 255 };
    drawText("FLAPPY BIRD", SCREEN_WIDTH / 2 - 120, 100, white);//Vẽ tiêu đề “FLAPPY BIRD” ở tọa độ (SCREEN_WIDTH/2 - 120, 100) — gần giữa màn hình theo chiều ngang, cao 100 pixel tính từ trên xuống.
    drawText("Press ENTER to Play", SCREEN_WIDTH / 2 - 140, 250, white);
    drawText("Press R to View Ranking", SCREEN_WIDTH / 2 - 150, 310, white);
    drawText("Press ESC to Exit", SCREEN_WIDTH / 2 - 130, 370, white);
    //SCREEN_WIDTH / 2-offset là tọa độ X ở chính giữa màn hình.
    //OFFSET là số pixel mà bạn lùi lại sang trái để văn bản có thể hiển thị đúng giữa màn hình.
}

void drawRankingScreen() {// vẽ bảng ranking
    SDL_RenderCopy(g_renderer, rankingBackground, nullptr, nullptr);//lấy toàn bộ ảnh ranking rồi dùng g_render đè lên

    SDL_Color yellow = { 255, 255, 0 };
    drawText("Top 5 High Scores", SCREEN_WIDTH / 2 - 150, 50, yellow);//màu vàng, canh giữa ngang, nằm ở khoảng cách 50px từ đỉnh màn hình

    int startY = 150;//Nghĩa là dòng đầu tiên sẽ được vẽ cách mép trên màn hình 150 pixels.
    for (int i = 0; i < highScores.size(); ++i) {
        std::string line = std::to_string(i + 1) + ". " + std::to_string(highScores[i]);
        drawText(line, SCREEN_WIDTH / 2 - 60, startY + i * 50, yellow);// gọi drawtext để vẽ chữ lên mh,startY + i * 50
       // Mỗi dòng cách nhau 50 pixels
    }

    SDL_Color white = { 255, 255, 255 };
    drawText("Press ESC to return", SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT - 80, white);
}

int main(int argc, char* argv[]) {
    if (!init() || !loadMedia()) return -1;//gọi hàm khởi tạo SDL, tạo cửa sổ, renderer, font... 

    bool quit = false;
    SDL_Event e;//SDL_Event – cấu trúc dùng để nhận thông tin về các sự kiện từ người

    Uint32 lastTime = SDL_GetTicks();//SDL_GetTicks() trả về số mili giây kể từ khi SDL được khởi động.
   // lastTime dùng để đo thời gian trôi qua giữa các frame

    while (!quit) {
        while (SDL_PollEvent(&e)) {//ktra nếu có sự kiện thì lưu vào biến e
            if (e.type == SDL_QUIT) quit = true;// khi người chơi làm sự kiện quit đặt quit = true để thoát game

            else if (gameMenu && e.type == SDL_KEYDOWN) {//nếu đang ở menu,sự kiện nhấn phím
                if (e.key.keysym.sym == SDLK_RETURN) {//nếu vừa nhấn enter,e.key.keysym.sym là mã của phím vừa được nhấn
                    gameMenu = false;//thoát khỏi menu
                    showRanking = false;//tắt bảng rank
                    bird.Reset();//reset lại vị trí của chim
                    pipeManager.Reset();//reset lại điểm số xoá các ống cũ
                    Mix_PlayMusic(backgroundMusic, -1); // phát nhạc nền lặp mãi mãi
                    lastTime = SDL_GetTicks();//cập nhật lại thời gian hiện tại để dùng cho tính toán cập nhật frame.
                }
                else if (e.key.keysym.sym == SDLK_r) {//nếu nhấn R hiênh bảng xếp hạng
                    showRanking = true;
                }
                else if (e.key.keysym.sym == SDLK_ESCAPE) {// nếu nhấn esc
                    if (showRanking) showRanking = false;// nếu đang ở ranking , thoát bảng điểm
                    else quit = true;// o phải thì thoát game
                }
            }

            else if (!gameMenu && !gameOver && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE) {// nếu đang o ở menu,game chưa end,sự kiện nhấn phím,nếu là phím cách
                bird.Jump();//chim nhảy lên
                Mix_PlayChannel(-1, jumpSound, 0);// phát âm thanh nhảy, 0 là phát 1 lần,1 là phát 2 lần,-1 là vô tận
            }

            else if (gameOver && e.type == SDL_KEYDOWN) {// nếu game end, sự kiện nhấn phím
                if (e.key.keysym.sym == SDLK_r) {// nhấn R
                    bird.Reset();//Đưa con chim trở lại vị trí ban đầu, đặt lại vận tốc (velocity = 0).
                    pipeManager.Reset();//xóa các ống cũ, reset điểm số và khởi tạo lại thời gian tạo pipe.
                    gameOver = false;// thoát trạng thái game over
                    Mix_PlayMusic(backgroundMusic, -1);//phát nhạc nền vô tận
                    lastTime = SDL_GetTicks();//Lưu thời gian hiện tại để bắt đầu tính thời gian update
                }
                else if (e.key.keysym.sym == SDLK_ESCAPE) {// nếu game end, nhấn esc
                    gameMenu = true;// trở vêf menu
                    gameOver = false;// tắt trạng thái gameover
                }
            }
        }

        SDL_SetRenderDrawColor(g_renderer, 0, 0, 0, 255);// làm sạch màn hình mỗi frame bằng màu đen, chuẩn bị để vẽ mới mọi thứ (nền, chim, ống, v.v).
       // Điều này giúp tránh việc vẽ đè lên khung hình cũ
        SDL_RenderClear(g_renderer);

        if (gameMenu) {
            if (showRanking) drawRankingScreen();//Nếu đang bật bảng xếp hạng (showRanking = true), gọi hàm drawRankingScreen() để vẽ bảng xếp hạng.
            else drawMenu();
        }
        else {
            if (!gameOver) {// nếu o ở menu và chưa thua
            //now: thời điểm hiện tại(tính bằng mili - giây kể từ khi game chạy).
               // delta : thời gian trôi qua từ frame trước(chuyển thành giây).
              //  lastTime = now; cập nhật lại mốc thời gian cho frame kế tiếp.
                Uint32 now = SDL_GetTicks();
                float delta = (now - lastTime) / 1000.0f;//1000.0f số mili giây trong 1 giây,delta là thời gian trôi qua giữa 2 lần cập nhật game
                lastTime = now;

                bird.Update();//gọi hàm cập nhật trạng thái chim và ống 
                pipeManager.Update();

                if (pipeManager.CheckCollision(bird.rect) ||//hàm này kiểm tra xem có sự va chạm nào giữa chim (bird.rect) và các ống (pipes) hay không. Nếu có, điều kiện này trả về true.
                    bird.rect.y + bird.rect.h >= SCREEN_HEIGHT ||//Nếu vị trí y của chim (bird.rect.y) cộng với chiều cao của chim (bird.rect.h) lớn hơn hoặc bằng chiều cao màn hình (SCREEN_HEIGHT)
                    bird.rect.y <= 0) // tức là va chạm
                {
                    Mix_HaltMusic();// dừng nhạc nền
                    Mix_PlayChannel(-1, collisionSound, 0);// phát âm thanh va chạm 1 lần
                    gameOver = true;

                    highScores.push_back(pipeManager.GetScore());// thêm điểm số hiện tại vào dsach,pipeManager.GetScore() trả về điểm số hiện tại đạt được
                    std::sort(highScores.rbegin(), highScores.rend());// sắp xếp điểm cao nhất đến bé nhất
                    if (highScores.size() > 5) highScores.resize(5);//nếu số lượng điểm cao vượt quá 5, chỉ giữ lại 5 điểm cao nhất
                }
            }

            SDL_RenderCopy(g_renderer, backgroundTexture, nullptr, nullptr);
            //Gọi hàm Render của pipeManager để vẽ các ống (pipes) trên màn hình. pipeTexture là texture mà bạn đã tải cho các ống trong game.
            pipeManager.Render(g_renderer, pipeTexture);
            SDL_RenderCopy(g_renderer, birdTexture, nullptr, &bird.rect);// vẽ ảnh của chim lên mh = texture của chim
            drawScore();// vẽ điểm cao nhất của ng chơi

            if (gameOver) {
                SDL_Color red = { 255, 0, 0 };//Tạo một đối tượng màu đỏ để sử dụng cho văn bản.
                drawText("Game Over! Press R to Restart", SCREEN_WIDTH / 2 - 200, 200, red);// vẽ thông báo lên mh
                drawText("Press ESC to return to Menu", SCREEN_WIDTH / 2 - 200, 260, red);
            }
        }

        SDL_RenderPresent(g_renderer);// cập nhật cửa sổ hiển thị sau khi vẽ
        SDL_Delay(16);//Dừng chương trình trong 16 milliseconds (~60 FPS) để đảm bảo trò chơi chạy với tốc độ ổn định và không quá nhanh
    }

    close();
    return 0;
}
