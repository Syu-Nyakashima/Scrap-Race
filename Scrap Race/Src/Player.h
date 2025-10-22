#pragma once
#include "DxLib.h"

class Player
{
public:
    void Player_Initialize();
    void Player_Terminate();
    void Player_Update(float delta);
    void Player_Draw();                     

    VECTOR GetPosition() const { return pos; }  // プレイヤーの座標取得
    VECTOR pos;

    float angle;
    float moveSpeed;

private:
    int ModelHandle;

    struct Transform {
        VECTOR position;
        VECTOR rotation; // (x, y, z) 回転角度
        VECTOR scale;
        MATRIX world;
    } transform;
};