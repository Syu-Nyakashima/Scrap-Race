#include "CarBase.h"
#include <math.h>

CarBase::CarBase(Stage& stageRef)
	: stage(stageRef), ModelHandle(-1), hitWall(false), wasHitWall(false), onGround(false)
{
	// メンバ変数の初期化のみ、Initializeで初期化
}

CarBase::~CarBase()
{
	if (ModelHandle != -1) {
		MV1DeleteModel(ModelHandle);
		ModelHandle = -1;
	}
}

void CarBase::Initialize() 
{
    // 位置初期化
    pos = VGet(0.0f, 10.0f, 0.0f);
    vel = VGet(0.0f, 0.0f, 0.0f);
    angle = 0.0f;

    // ステータス初期化
    moveSpeed = 0.0f;
    SpdMax = 150.0f;
    SpdMin = 0.0f;
    SpdUp = 0.5f;
    SpdDown = 0.5f;
    Hp = 100.0f;

    // 当たり判定初期化
    capsuleRadius = 1.5f;
    capsuleHeight = 2.0f;

    // 状態フラグ初期化
    hitWall = false;
    wasHitWall = false;
    onGround = false;
}

void CarBase::Terminate()
{
    if (ModelHandle != -1) {
        MV1DeleteModel(ModelHandle);
        ModelHandle = -1;
    }
}

void CarBase::Draw() 
{
    if (ModelHandle == -1) return;

    MATRIX matScale = MGetScale(VGet(1.0f, 1.0f, 1.0f));
    MATRIX matRot = MGetRotY(angle * DX_PI_F / 180.0f);
    MATRIX matTrans = MGetTranslate(VAdd(pos, VGet(0.0f, 0.0f, 0.0f)));
    MATRIX matWorld = MMult(MMult(matScale, matRot), matTrans);

    MV1SetMatrix(ModelHandle, matWorld);
    MV1DrawModel(ModelHandle);
}

void CarBase::Heal(float amount) 
{
    Hp += amount;
    if (Hp > 100.0f) {
        Hp = 100.0f;
    }
}
/*
void CarBase::BoostStatus(float spdMaxBoost, float spdUpBoost, float spdDownBoost)
{
    SpdMax += spdMaxBoost;
    SpdUp += spdUpBoost;
    SpdDown += spdDownBoost;

    // 上限設定(必要なら)
    if (SpdMax > 250.0f) SpdMax = 250.0f;
    if (SpdUp > 2.0f) SpdUp = 2.0f;
    if (SpdDown > 2.0f) SpdDown = 2.0f;
}

void CarBase::DrainStatusOverTime(float delta)
{
    // ステータス減少(下限あり)
    SpdMax -= STATUS_DRAIN_SPD_MAX * delta;
    if (SpdMax < MIN_SPD_MAX) SpdMax = MIN_SPD_MAX;

    SpdUp -= STATUS_DRAIN_SPD_UP * delta;
    if (SpdUp < MIN_SPD_UP) SpdUp = MIN_SPD_UP;

    SpdDown -= STATUS_DRAIN_SPD_DOWN * delta;
    if (SpdDown < MIN_SPD_DOWN) SpdDown = MIN_SPD_DOWN;
}
*/
void CarBase::UpdatePhysics(float delta)
{
    // 角度から進行方向を計算
    float rad = angle * DX_PI_F / 180.0f;

    // 速度計算
    vel.x = sinf(rad) * moveSpeed * delta;
    vel.z = cosf(rad) * moveSpeed * delta;
    vel.y += GRAVITY * delta;

    // 位置更新
    pos = VAdd(pos, vel);
}

void CarBase::UpdateCollision(float delta) 
{
    //ステージのコリジョン判定
    int colModel = stage.GetCheckColModel();

    // 地面を判定
    CheckGround(colModel, delta);
    //壁を判定
    CheckWall(colModel, delta);
}

void CarBase::CheckGround(int CheckColModel, float delta)
{
    if (CheckColModel == -1) return;

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

    for (int i = 0; i < CHECK_POINTS; i++)
    {
        VECTOR checkPos = VAdd(pos, checkOffsets[i]);
        float groundY;

        if (CheckGroundPoint(CheckColModel, checkPos, groundY)) {
            // 最も高い地面を記録
            if (groundY > highestGroundY) {
                highestGroundY = groundY;
                foundGround = true;
            }
        }
    }

    // 地面の処理
    if (foundGround) {
        ApplyGroundPhysics(highestGroundY, delta);
    }
    else {
        onGround = false;
    }
}

bool CarBase::CheckGroundPoint(int CheckColModel, VECTOR checkPos, float& outGroundY)
{
    VECTOR lineStart = VAdd(checkPos, VGet(0.0f, 5.0f, 0.0f));
    VECTOR lineEnd = VAdd(checkPos, VGet(0.0f, -50.0f, 0.0f));

    MV1_COLL_RESULT_POLY HitPoly = MV1CollCheck_Line(
        CheckColModel, -1, lineStart, lineEnd
    );

    // デバッグ表示
    int lineColor = (HitPoly.HitFlag == 1) ? GetColor(0, 255, 0) : GetColor(255, 0, 0);
    DrawLine3D(lineStart, lineEnd, lineColor);

    if (HitPoly.HitFlag == 1)
    {
        outGroundY = HitPoly.HitPosition.y;
        return true;
    }
}

void CarBase::ApplyGroundPhysics(float groundY, float delta)
{
    // 車の足元の高さ
    const float FOOT_OFFSET = capsuleHeight * FOOT_OFFSET_RATIO;
    float targetY = groundY + FOOT_OFFSET;

    // 地面より下、または非常に近い場合
    if (pos.y <= targetY + 0.05f)  // 許容範囲を小さく
    {
        pos.y = targetY;

        //空中なら
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
}

void CarBase::CheckWall(int CheckColModel, float delta)
{
    if (CheckColModel == -1) return;

    const float WALL_RADIUS = capsuleRadius;
    hitWall = false;

    for (int iteration = 0; iteration < MAX_WALL_ITERATIONS; iteration++)
    {
        VECTOR capsuleTop = VAdd(pos, VGet(0.0f, capsuleHeight * 0.4f, 0.0f));
        VECTOR capsuleBottom = VAdd(pos, VGet(0.0f, -capsuleHeight * 0.2f, 0.0f));

        MV1_COLL_RESULT_POLY_DIM HitPolyDim = MV1CollCheck_Capsule(
            CheckColModel, -1, capsuleTop, capsuleBottom, WALL_RADIUS
        );

        //壁に当たっていなければ判定を消す
        if (HitPolyDim.HitNum == 0)
        {
            MV1CollResultPolyDimTerminate(HitPolyDim);
            break;
        }

        //壁に当たったとき
        ProcessWallCollision(HitPolyDim);

        MV1CollResultPolyDimTerminate(HitPolyDim);

        if (!hitWall)
        {
            break;
        }
    }

    //壁衝突ダメージ
    ApplyWallDamage();

    wasHitWall = hitWall;
}

void CarBase::ProcessWallCollision(const MV1_COLL_RESULT_POLY_DIM& HitPolyDim)
{
    const float WALL_RADIUS = capsuleRadius;

    VECTOR totalPushOut = VGet(0.0f, 0.0f, 0.0f);
    VECTOR avgNormal = VGet(0.0f, 0.0f, 0.0f);
    int validNormalCount = 0;

    for (int i = 0; i < HitPolyDim.HitNum; i++)
    {
        MV1_COLL_RESULT_POLY& poly = HitPolyDim.Dim[i];
        VECTOR normal = poly.Normal;

        // 地面や天井を除外
        if (fabsf(normal.y) > 0.5f) continue;

        VECTOR toCenter = VSub(pos, poly.Position[0]);
        float dist = VDot(toCenter, normal);
        float penetration = WALL_RADIUS - dist;

        if (penetration > 0.0f)
        {
            totalPushOut = VAdd(totalPushOut, VScale(normal, penetration));
            avgNormal = VAdd(avgNormal, normal);
            validNormalCount++;
        }
    }

    if (VSize(totalPushOut) > 0.001f)
    {
        float pushDistance = VSize(totalPushOut);
        //異常な押し出しの検出
        if (pushDistance > 10.0f)
        {
            printfDx("WARNING: Large push detected: %.2f\n", pushDistance);
            pos = VGet(0.0f, 20.0f, 0.0f);
            vel = VGet(0.0f, 0.0f, 0.0f);
            moveSpeed = 0.0f;
            hitWall = false;
            return;
        }

        //平均法線を計算
        if (validNormalCount > 0) {
            avgNormal = VScale(avgNormal, 1.0f / validNormalCount);
            avgNormal = VNorm(avgNormal);
        }

        //押し出し
        ApplyWallPushOut(totalPushOut, avgNormal);

        hitWall = true;
    }
}

void CarBase::ApplyWallPushOut(VECTOR pushOut, VECTOR avgNormal)
{
    //位置の押し出し
    pos = VAdd(pos, VScale(pushOut, 1.1f));

    // 速度ベクトルを反射
    float velDot = VDot(vel, avgNormal);

    if (velDot < 0.0f)
    {
        VECTOR reflection = VScale(avgNormal, velDot * (1.0f + RESTITUTION));
        vel = VSub(vel, reflection);

        // moveSpeedを反転
        moveSpeed *= -RESTITUTION;
    }
}

void CarBase::ApplyWallDamage()
{
    // 壁に当たった瞬間のHP減少
    if (hitWall && !wasHitWall)
    {
        float currentSpeed = fabsf(moveSpeed);

        //HP減少
        float damage = currentSpeed * DAMAGE_MULTIPLIER;
        Hp -= damage;
        printfDx("%f減少\n", damage);
    }
}