#pragma once
#include "DxLib.h"

class Player
{
public:
    VECTOR pos;
    float angle;
    float moveSpeed;

    void Player_Initialize();
    void Player_Terminate();
    void Chara_Player_Create();
};