#include "Player.h"
#include <math.h>

void Player::Player_Initialize()
{
    pos = VGet(0.0f, 0.0f, 0.0f);
    angle = 0.0f;
    moveSpeed = 0.0f;
    SpdMax = 150.0f;
    SpdMin = 0.0;
    SpdUp = 0.5f;
    SpdDown = 0.5f;
    Hp = 100.0f;
    
    ModelHandle = MV1LoadModel("Data/Model/free_car_1.mv1");
    if (ModelHandle == -1) printfDx("モデル読み込み失敗！\n");
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
    Hp -= 0.01f;   
    if (Hp <= 0.0f) {
        Hp = 0.0f;
    }

    // 左右回転
    if (CheckHitKey(KEY_INPUT_LEFT))  angle -= 180.0f * delta;
    if (CheckHitKey(KEY_INPUT_RIGHT)) angle += 180.0f * delta;

    // 前後移動
    float rad = angle * DX_PI_F / 180.0f;

    //加速
    if (CheckHitKey(KEY_INPUT_UP)) {
        moveSpeed += SpdUp;

        if (moveSpeed >= SpdMax) moveSpeed = SpdMax;
    }

    else if(CheckHitKey(KEY_INPUT_DOWN)){
        //停止時バック
        if (moveSpeed <= 0.0f) {
            moveSpeed -= SpdUp;
            if (moveSpeed < -SpdMax * 0.5f) moveSpeed = -SpdMax * 0.5f;
        }
        //停止してなかったらブレーキ
        else {
            moveSpeed -= SpdDown*2.0f;
            if (moveSpeed < 0.0f) moveSpeed = 0.0f;
        }
    }

    else{
        //何も押してなかったら減速
        if (moveSpeed > 0) {
            moveSpeed -= SpdDown;
            if (moveSpeed < 0.0f) moveSpeed = 0.0f;
        }
        //バック時
        else if (moveSpeed < 0) {
            moveSpeed += SpdUp;
            if (moveSpeed > 0.0f) moveSpeed = 0.0f;
        }
    }

    //計算
    if (moveSpeed != 0) {
        pos.x += sinf(rad) * moveSpeed * delta;
        pos.z += cosf(rad) * moveSpeed * delta;
    }
}

void Player::Player_Draw()
{
    if (ModelHandle == -1) return;

    //モデル制御 

    MATRIX matScale = MGetScale(VGet(1.0f, 1.0f, 1.0f));
    MATRIX matRot = MGetRotY(angle * DX_PI_F / 180.0f);
    MATRIX matTrans = MGetTranslate(VAdd(pos, VGet(0.0f, 0.0f, 0.0f)));
    MATRIX matWorld = MMult(MMult(matScale, matRot), matTrans);

    MV1SetMatrix(ModelHandle, matWorld);
    MV1DrawModel(ModelHandle);

    // デバッグ用球（必ず見える）
    //DrawSphere3D(pos, 2.0f, 16, GetColor(255, 0, 0), GetColor(255, 0, 0), TRUE);
}

void Player::Heal(float amount)
{
    Hp += amount;
    Hp = (Hp > 100.0f) ? 100.0f : Hp;
   // if (Hp > 100.0f) Hp = 100.0f;
}
