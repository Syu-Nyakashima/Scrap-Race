#include "Player.h"
#include <math.h>

Player::Player(Stage& stageRef) 
    : CarBase(stageRef)
{
    //Initializeで初期化
}

Player::~Player()
{

}

//ゲーム内の初期化
void Player::Initialize()
{
    CarBase::Initialize();

    //モデル読み込み
    ModelHandle = MV1LoadModel("Data/Model/free_car_01.mv1");
    if (ModelHandle == -1) printfDx("モデル読み込み失敗！\n");
    
    // 位置初期化
    pos = VGet(0.0f, 2.0f, 0.0f);
    SpherePos = pos;
    vel = VGet(0.0f, 0.0f, 0.0f);
    angle = -90.0f;

    //ステータス初期化
    moveSpeed = 0.0f;
    SpdMax = 100.0f;
    SpdMin = 0.0f;
    SpdUp = 0.5f;
    SpdDown = 0.5f;
    Hp = 100.0f;

    //当たり判定初期化
    hitWall = false;
    wasHitWall = false;
    onGround = false;
}

void Player::Terminate() {
    CarBase::Terminate();
}

void Player::Update(float delta)
{
    // HP自然減少
    Hp -= HP_DRAIN_PER_FRAME;
    if (Hp <= 0.0f) {
        Hp = 0.0f;
    }
    //時間経過でステータス減少
    DrainStatusOverTime(delta);
    //入力処理
    UpdateInput(delta);
    //物理演算
    UpdatePhysics(delta);
    //衝突判定
    UpdateCollision(delta);
}

void Player::UpdateInput(float delta)
{
    HandleRotation(delta);
    HandleAcceleration(delta);
}

void Player::HandleRotation(float delta)
{
    if (CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A)) {
        angle -= ROTATION_SPEED * delta;
    }
    if (CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_D)) {
        angle += ROTATION_SPEED * delta;
    }
}

void Player::HandleAcceleration(float delta)
{
    {
        // 前進
        if (CheckHitKey(KEY_INPUT_UP) || CheckHitKey(KEY_INPUT_W)) {
            moveSpeed += SpdUp;
            if (moveSpeed >= SpdMax) moveSpeed = SpdMax;
        }
        // 後退・ブレーキ
        else if (CheckHitKey(KEY_INPUT_DOWN) || CheckHitKey(KEY_INPUT_S)) {
            //後退
            if (moveSpeed <= 0.0f) {
                moveSpeed -= SpdUp;
                if (moveSpeed < -SpdMax * 0.5f) moveSpeed = -SpdMax * 0.5f;
            }
            //ブレーキ
            else {
                moveSpeed -= SpdDown * 2.0f;
                if (moveSpeed < 0.0f) moveSpeed = 0.0f;
            }
        }
        //何も押していない（自然減速）
        else {
            if (moveSpeed > 0) {
                moveSpeed -= SpdDown;
                if (moveSpeed < 0.0f) moveSpeed = 0.0f;
            }
            else if (moveSpeed < 0) {
                moveSpeed += SpdUp;
                if (moveSpeed > 0.0f) moveSpeed = 0.0f;
            }
        }
    }

}

void Player::Draw()
{
    CarBase::Draw();
}