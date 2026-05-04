#include "Bird.h"
#include "Constants.h"
//Dòng này bao g?m file khai báo l?p Bird và file ch?a các h?ng s? nh? SCREEN_HEIGHT, GRAVITY, JUMP_STRENGTH.
Bird::Bird() {
    //rect là m?t SDL_Rect ??i di?n cho hình ch? nh?t bao quanh chim (x, y, w, h)
    rect.w = 50;
    rect.h = 50;
    //con chim ban ??u có kích th??c 50x50
    Reset();//G?i Reset() ?? ??t l?i v? trí ban ??u c?a chim và t?c ?? r?i
}

void Bird::Update() {//Hàm Update() ???c g?i m?i frame ?? c?p nh?t v? trí chim
    velocity += GRAVITY;//v?n t?c r?i c?a chim, t?ng d?n b?i GRAVITY ,GRAVITY là m?t h?ng s? d??ng nh?
    //velocity là t?c ?? di chuy?n c?a chim theo tr?c y (lên/xu?ng).
    // lúc ??u v=0,gra=0,5,frame 1: 0+ 0,5,frame 2:0,5+0,5...
    rect.y += static_cast<int>(velocity);//C?p nh?t v? trí Y c?a chim theo v?n t?c., rect.y t?ng, chim ?i xu?ng,
    //Gi? s?
   // velocity = 1.8 ta ép thành 1(int)
      //  rect.y = 100,rect y=101, chim r?i xu?ng 1 pixel

}

void Bird::Jump() {
    velocity = JUMP_STRENGTH;// khi nh?y gán velocity = jump_tr ( là 1 s? âm) nên khi  rect.y += velocity (gi?m)s? làm chim ?i lên.
}

void Bird::Reset() {// ??a v? tr?ng thái ban ??u khi ng ch?i reset
    rect.x = 200;
    rect.y = SCREEN_HEIGHT / 2 - rect.h / 2;
    velocity = 0;//velocity = 0; ? chim ??ng yên, không r?i c?ng không bay ngay t? ??u.
}
