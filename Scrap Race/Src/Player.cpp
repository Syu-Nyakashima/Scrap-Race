#include "Player.h"
#include "DxLib.h"

void Player::Player_Initialize()
{
    pos = VGet(0.0f, 0.0f, 0.0f);
    angle = 0.0f;
    moveSpeed = 30.0f;
}

void Player::Player_Terminate()
{
    // 必要ならモデル解放など
}

void Player::Chara_Player_Create()
{
    // 簡易的な移動
    if (CheckHitKey(KEY_INPUT_UP)) pos.z += moveSpeed * 0.1f;
    if (CheckHitKey(KEY_INPUT_DOWN)) pos.z -= moveSpeed * 0.1f;
    if (CheckHitKey(KEY_INPUT_LEFT)) pos.x -= moveSpeed * 0.1f;
    if (CheckHitKey(KEY_INPUT_RIGHT)) pos.x += moveSpeed * 0.1f;

    // 簡易描画
    DrawSphere3D(pos, 5.0f, 16, GetColor(255, 128, 128), GetColor(255, 128, 128), TRUE);
}