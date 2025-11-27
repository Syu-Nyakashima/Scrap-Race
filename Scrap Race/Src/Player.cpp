#include "Player.h"
#include <math.h>

Player::Player(Stage& stageRef) : stage(stageRef), ModelHandle(-1)
{
    //Initializeで初期化
}

Player::~Player()
{
    if (ModelHandle != -1) {
        MV1DeleteModel(ModelHandle);
        ModelHandle = -1;
    }
}

//ゲーム内の初期化
void Player::Initialize()
{
    pos = VGet(0.0f, 10.0f, 0.0f);
    SpherePos = pos;
    vel = VGet(0.0f, 0.0f, 0.0f);
    angle = 0.0f;
    moveSpeed = 0.0f;
    SpdMax = 150.0f;
    SpdMin = 0.0f;
    SpdUp = 0.5f;
    SpdDown = 0.5f;
    Hp = 100.0f;

    ModelHandle = MV1LoadModel("Data/Model/free_car_1.mv1");
    if (ModelHandle == -1) printfDx("モデル読み込み失敗！\n");
}

void Player::Update(float delta)
{
    Hp -= 0.01f;
    if (Hp <= 0.0f) {
        Hp = 0.0f;
    }

    // 左右回転
    if (CheckHitKey(KEY_INPUT_LEFT))  angle -= 180.0f * delta;
    if (CheckHitKey(KEY_INPUT_RIGHT)) angle += 180.0f * delta;

    float rad = angle * DX_PI_F / 180.0f;

    // 加速・減速
    {
        if (CheckHitKey(KEY_INPUT_UP)) {
            moveSpeed += SpdUp;
            if (moveSpeed >= SpdMax) moveSpeed = SpdMax;
        }
        else if (CheckHitKey(KEY_INPUT_DOWN)) {
            if (moveSpeed <= 0.0f) {
                moveSpeed -= SpdUp;
                if (moveSpeed < -SpdMax * 0.5f) moveSpeed = -SpdMax * 0.5f;
            }
            else {
                moveSpeed -= SpdDown * 2.0f;
                if (moveSpeed < 0.0f) moveSpeed = 0.0f;
            }
        }
        //何も押していない
        else {
            float deceleration = hitWall ? SpdDown * 2.0f : SpdDown;
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

    //速度＋重力計算
    vel.x = sinf(rad) * moveSpeed * delta;
    vel.z = cosf(rad) * moveSpeed * delta;
    vel.y += -9.8f * delta;

    // 位置更新
    pos = VAdd(pos, vel);

    // 地面判定
    CheckGround(stage.GetCheckColModel(), delta);

    //壁判定
    CheckWall(stage.GetCheckColModel(), delta);
}

void Player::Draw()
{
    if (ModelHandle == -1) return;

    MATRIX matScale = MGetScale(VGet(1.0f, 1.0f, 1.0f));
    MATRIX matRot = MGetRotY(angle * DX_PI_F / 180.0f);
    MATRIX matTrans = MGetTranslate(VAdd(pos, VGet(0.0f, 0.0f, 0.0f)));
    MATRIX matWorld = MMult(MMult(matScale, matRot), matTrans);
    MV1SetMatrix(ModelHandle, matWorld);
    MV1DrawModel(ModelHandle);
}

void Player::Heal(float amount)
{
    Hp += amount;
    Hp = (Hp > 100.0f) ? 100.0f : Hp;
}

void Player::CheckGround(int CheckColModel, float delta)
{
    if (CheckColModel == -1) return;

    // 車の足元の高さ
    const float FOOT_OFFSET = capsuleHeight * 0.5f;

    // 複数の点から線判定（窪地対策）
    const int CHECK_POINTS = 4;
    VECTOR checkOffsets[CHECK_POINTS] = {
        VGet(0.0f, 0.0f, 0.0f),           // 中央
        VGet(capsuleRadius * 0.5f, 0.0f, 0.0f),     // 右
        VGet(-capsuleRadius * 0.5f, 0.0f, 0.0f),    // 左
        VGet(0.0f, 0.0f, capsuleRadius * 0.5f)      // 前
    };

    float highestGroundY = -99999.0f;
    bool foundGround = false;
    VECTOR hitPosition = VGet(0.0f, 0.0f, 0.0f);

    for (int i = 0; i < CHECK_POINTS; i++)
    {
        VECTOR checkPos = VAdd(pos, checkOffsets[i]);
        VECTOR lineStart = VAdd(checkPos, VGet(0.0f, 5.0f, 0.0f));
        VECTOR lineEnd = VAdd(checkPos, VGet(0.0f, -50.0f, 0.0f));

        MV1_COLL_RESULT_POLY HitPoly = MV1CollCheck_Line(
            CheckColModel, -1, lineStart, lineEnd
        );

        if (HitPoly.HitFlag == 1)
        {
            float groundY = HitPoly.HitPosition.y;

            // 最も高い地面を記録
            if (groundY > highestGroundY)
            {
                highestGroundY = groundY;
                hitPosition = HitPoly.HitPosition;
                foundGround = true;
            }
        }

        // デバッグ表示
        int lineColor = (HitPoly.HitFlag == 1) ? GetColor(0, 255, 0) : GetColor(255, 0, 0);
        DrawLine3D(lineStart, lineEnd, lineColor);
    }

    // 地面の処理
    if (foundGround)
    {
        float targetY = highestGroundY + FOOT_OFFSET;

        // 地面より下、または非常に近い場合
        if (pos.y <= targetY + 0.05f)  // 許容範囲を小さく
        {
            pos.y = targetY;

            if (vel.y < 0.0f)
            {
                vel.y = 0.0f;
            }
            onGround = true;
        }
        else
        {
            onGround = false;
        }

        // 当たった位置を表示
        DrawSphere3D(hitPosition, 0.5f, 8, GetColor(255, 255, 0), GetColor(255, 255, 0), TRUE);
    }
    else
    {
        onGround = false;
    }
}

void Player::CheckWall(int CheckColModel, float delta)
{
    if (CheckColModel == -1) return;

    const float WALL_RADIUS = capsuleRadius;
    float speed = sqrtf(vel.x * vel.x + vel.z * vel.z);

    // 速度制限
    const float MAX_SAFE_SPEED = WALL_RADIUS * 2.0f;  // 少し緩め

    if (speed > MAX_SAFE_SPEED)
    {
        float scale = MAX_SAFE_SPEED / speed;
        vel.x *= scale;
        vel.z *= scale;
    }

    // 壁判定
    const int MAX_ITERATIONS = 3;
    hitWall = false;  // このフレームで壁に当たったかをリセット

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++)
    {
        VECTOR capsuleTop = VAdd(pos, VGet(0.0f, capsuleHeight * 0.4f, 0.0f));
        VECTOR capsuleBottom = VAdd(pos, VGet(0.0f, -capsuleHeight * 0.2f, 0.0f));

        MV1_COLL_RESULT_POLY_DIM HitPolyDim = MV1CollCheck_Capsule(
            CheckColModel, -1, capsuleTop, capsuleBottom, WALL_RADIUS
        );

        if (HitPolyDim.HitNum == 0)
        {
            MV1CollResultPolyDimTerminate(HitPolyDim);
            break;
        }

        VECTOR totalPushOut = VGet(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < HitPolyDim.HitNum; i++)
        {
            MV1_COLL_RESULT_POLY& poly = HitPolyDim.Dim[i];
            VECTOR normal = poly.Normal;

            if (fabsf(normal.y) > 0.5f) continue;

            VECTOR toCenter = VSub(pos, poly.Position[0]);
            float dist = VDot(toCenter, normal);
            float penetration = WALL_RADIUS - dist;

            if (penetration > 0.0f)
            {
                totalPushOut = VAdd(totalPushOut, VScale(normal, penetration));
            }
        }

        MV1CollResultPolyDimTerminate(HitPolyDim);

        if (VSize(totalPushOut) > 0.001f)
        {
            pos = VAdd(pos, VScale(totalPushOut, 1.1f));

            VECTOR pushDir = VNorm(totalPushOut);
            float velDot = VDot(vel, pushDir);

            if (velDot < 0.0f)
            {
                vel = VSub(vel, VScale(pushDir, velDot));
            }

            hitWall = true;  // 壁に当たった
        }
        else
        {
            break;
        }
    }

    // 壁に当たった瞬間の処理
    if (hitWall && !wasHitWall)
    {
        // HP減少
        float currentSpeed = fabsf(moveSpeed);
        float MinusHp = currentSpeed * 0.1f;
        Hp -= MinusHp;
        printfDx("%.2fHP減少\n", MinusHp);
    }

    // 壁に当たっている間の処理
    if (hitWall)
    {
        // 減速率
        const float SPEED_REDUCTION = 0.6f;
        moveSpeed *= SPEED_REDUCTION;
        printfDx("%.2f速度減少\n", moveSpeed);

    }

    // 次フレーム用に状態を保存
    wasHitWall = hitWall;
}
