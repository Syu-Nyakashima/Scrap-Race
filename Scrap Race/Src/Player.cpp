#include "Player.h"
#include "DxLib.h"
#include <math.h>

void Player::Player_Initialize()
{
    pos = VGet(0.0f, 0.0f, 0.0f);
    angle = 0.0f;
    moveSpeed = 30.0f;

    ModelHandle = MV1LoadModel("free_car_1.x");
    if (ModelHandle == -1) printfDx("モデル読み込み失敗！\n");

    MV1SetScale(ModelHandle, VGet(1.0f, 1.0f, 1.0f));
}

void Player::Player_Terminate()
{
    if (ModelHandle != -1) {
        MV1DeleteModel(ModelHandle);
        ModelHandle = -1;
    }
}

void Player::Player_Update(float delta)
{
    // 左右回転
    if (CheckHitKey(KEY_INPUT_LEFT))  angle -= 180.0f * delta;
    if (CheckHitKey(KEY_INPUT_RIGHT)) angle += 180.0f * delta;

    // 前後移動
    float rad = angle * DX_PI_F / 180.0f;

    if (CheckHitKey(KEY_INPUT_UP)) {
        pos.x += sinf(rad) * moveSpeed * delta;
        pos.z += cosf(rad) * moveSpeed * delta;
    }

    if (CheckHitKey(KEY_INPUT_DOWN)) {
        pos.x -= sinf(rad) * moveSpeed * delta;
        pos.z -= cosf(rad) * moveSpeed * delta;
    }
}

void Player::Player_Draw()
{
    if (ModelHandle == -1) return;

    // モデル原点が中心の場合はYを少し上げる
    MATRIX matRot = MGetRotY(angle * DX_PI_F / 180.0f);
    MATRIX matTrans = MGetTranslate(VAdd(pos, VGet(0.0f, 2.0f, 0.0f)));
    MATRIX matWorld = MMult(matRot, matTrans);

    MV1SetMatrix(ModelHandle, matWorld);
    MV1DrawModel(ModelHandle);

    // デバッグ用球（必ず見える）
    //DrawSphere3D(pos, 2.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
}