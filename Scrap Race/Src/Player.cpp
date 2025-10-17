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
    // �K�v�Ȃ烂�f������Ȃ�
}

void Player::Chara_Player_Create()
{
    // �ȈՓI�Ȉړ�
    if (CheckHitKey(KEY_INPUT_UP)) pos.z += moveSpeed * 0.1f;
    if (CheckHitKey(KEY_INPUT_DOWN)) pos.z -= moveSpeed * 0.1f;
    if (CheckHitKey(KEY_INPUT_LEFT)) pos.x -= moveSpeed * 0.1f;
    if (CheckHitKey(KEY_INPUT_RIGHT)) pos.x += moveSpeed * 0.1f;

    // �ȈՕ`��
    DrawSphere3D(pos, 5.0f, 16, GetColor(255, 128, 128), GetColor(255, 128, 128), TRUE);
}