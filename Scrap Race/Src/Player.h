#pragma once
#include "DxLib.h"

class Player
{
public:
    void Player_Initialize();
    void Player_Terminate();
    void Player_Update(float delta);
    void Player_Draw();                     

    VECTOR pos;
    float radius = 5.0f;

    float angle;
    float moveSpeed;
    float SpdUp;
    float SpdDown;
    float SpdMax;
    float SpdMin;

    float Hp = 100.0f;
    void Heal(float amount);

private:
    int ModelHandle;

    struct Transform {
        VECTOR position;
        VECTOR rotation; // (x, y, z) ‰ñ“]Šp“x
        VECTOR scale;
        MATRIX world;
    } transform;
};