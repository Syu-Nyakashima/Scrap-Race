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
    ModelHandle = MV1LoadModel("Data/Model/Car_Red.mv1");
    if (ModelHandle == -1) printfDx("モデル読み込み失敗！\n");
    
    // 位置初期化
    pos = VGet(0.0f, 2.0f, 0.0f);
    SpherePos = pos;
    vel = VGet(0.0f, 0.0f, 0.0f);
    angle = 0.0f;

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

    // HP減少によるスクラップ生成チェック
    CheckHPDropScrap();
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
    HandleDrift(delta);
    HandleRotation(delta);
    HandleAcceleration(delta);
}

void Player::HandleRotation(float delta)
{
    float speedRatio = fabsf(moveSpeed) / SpdMax;

    float rotSpeed;

    if (isDrifting)
    {
        // ドリフト中: 速度が速いほど素早く曲がれる
        // 基本旋回速度 × ブースト倍率 × 速度補正
        rotSpeed = NORMAL_ROTATION_SPEED * DRIFT_ROTATION_BOOST * (0.5f + speedRatio);

        // 例: 最高速度時
        // 60 * 2.0 * (0.5 + 1.0) = 180度/秒 (速い!)
    }
    else
    {
        // 通常旋回: 速度が速いと曲がりにくい（リアル志向）
        // 速度が速いほど旋回速度が下がる
        rotSpeed = NORMAL_ROTATION_SPEED * (1.5f - speedRatio);

        // 例: 
        // 低速時(speedRatio=0.2): 60 * 1.3 = 78度/秒
        // 高速時(speedRatio=1.0): 60 * 0.5 = 30度/秒 (曲がりにくい!)
    }

    if (CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A)) {
        angle -= rotSpeed * delta;
    }
    if (CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_D)) {
        angle += rotSpeed * delta;
    }
}

void Player::HandleAcceleration(float delta)
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

void Player::HandleDrift(float delta)
{
    if (CheckHitKey(KEY_INPUT_SPACE))
    {
		//ドリフト開始
        if (!isDrifting) {
            isDrifting = true;
            driftAngle = 0.0f;
        }

		//ドリフト中の挙動
        if(CheckHitKey(KEY_INPUT_LEFT) || CheckHitKey(KEY_INPUT_A))
        {
            //左ドリフト
            driftAngle -= DRIFT_ANGLE_SPEED * delta;
        }

        if(CheckHitKey(KEY_INPUT_RIGHT) || CheckHitKey(KEY_INPUT_D))
        {
            //右ドリフト
            driftAngle += DRIFT_ANGLE_SPEED * delta;  
		}

        // 速度に応じた最大角度（速いほど深いドリフト可能）
        float speedRatio = fabsf(moveSpeed) / SpdMax;
        float maxDriftAngle = 30.0f + (20.0f * speedRatio);

        if (driftAngle < -maxDriftAngle) driftAngle = -maxDriftAngle;
        if (driftAngle > maxDriftAngle) driftAngle = maxDriftAngle;

        // 横方向の速度を計算
        lateralVelocity = driftAngle * DRIFT_LATERAL_MULT;
    }
    else
    {
        //ドリフト終了
        if (isDrifting) {
            isDrifting = false;
        }
        //ドリフト角度を元に戻す
        driftAngle *= DRIFT_RECOVERY;
        lateralVelocity *= DRIFT_RECOVERY;

        if (fabs(driftAngle) < 0.1f) {
            driftAngle = 0.0f;
            lateralVelocity = 0.0f;
        }
	}
}

void Player::Draw()
{
    CarBase::Draw();
}