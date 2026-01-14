#include "CarBase.h"
#include <math.h>
#include "ItemManager.h"

CarBase::CarBase(Stage& stageRef)
	: stage(stageRef), ModelHandle(-1), hitWall(false), wasHitWall(false), onGround(false), justHitWall(false), allCars(nullptr)
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
    groundNormal = VGet(0.0f, 1.0f, 0.0f);

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
    isGoal = false;
    justHitWall = false;

    currentLap = 1;
    currentCheckpoint = 0;
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

    // スケール行列
    MATRIX matScale = MGetScale(VGet(1.0f, 1.0f, 1.0f));
    // Y軸回転(車の向き)
    MATRIX matRotY = MGetRotY(angle * DX_PI_F / 180.0f);
    // 地面の法線に合わせた回転行列を計算
    MATRIX matSlope = CalculateSlopeMatrix(groundNormal);
    // 回転を合成(傾斜 × Y軸回転)
    MATRIX matRot = MMult(matRotY, matSlope);
    // 移動行列
    MATRIX matTrans = MGetTranslate(VAdd(pos, VGet(0.0f, 0.0f, 0.0f)));
    // 最終的なワールド行列
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

void CarBase::BoostStatus(float spdMaxBoost, float spdUpBoost)
{
    SpdMax += spdMaxBoost;
    SpdUp += spdUpBoost;

    if (SpdMax > MAX_SPD_MAX) SpdMax = MAX_SPD_MAX;
    if (SpdUp > MAX_SPD_UP) SpdUp = MAX_SPD_UP;
}

void CarBase::DrainStatusOverTime(float delta)
{
    SpdMax -= STATUS_DRAIN_SPD_MAX * delta;
    if (SpdMax < MIN_SPD_MAX) SpdMax = MIN_SPD_MAX;

    SpdUp -= STATUS_DRAIN_SPD_UP * delta;
    if (SpdUp < MIN_SPD_UP) SpdUp = MIN_SPD_UP;
}

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
    SpherePos = pos;
}

void CarBase::UpdateCollision(float delta) 
{
    //ステージのコリジョン判定
    int colModel = stage.GetCheckColModel();
    justHitWall = false;

    // 地面を判定
    CheckGround(colModel, delta);
    //壁を判定
    CheckWall(colModel, delta);
    //車同士の接触判定
    CheckCarCollision(delta);
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
    VECTOR highestNormal = VGet(0.0f, 1.0f, 0.0f);
    bool foundGround = false;

    for (int i = 0; i < CHECK_POINTS; i++)
    {
        VECTOR checkPos = VAdd(pos, checkOffsets[i]);
        float groundY;
		VECTOR normal;

        if (CheckGroundPoint(CheckColModel, checkPos, groundY, normal)) {
            // 最も高い地面を記録
            if (groundY > highestGroundY) {
                highestGroundY = groundY;
                highestNormal = normal;
                foundGround = true;
            }
        }
    }

    // 地面の処理
    if (foundGround) {
        groundNormal = highestNormal;
        ApplyGroundPhysics(highestGroundY, delta);
    }
    else {
        onGround = false;

        groundNormal = VAdd(VScale(groundNormal, 0.95f), VScale(VGet(0.0f, 1.0f, 0.0f), 0.05f));
        groundNormal = VNorm(groundNormal);
    }
}

bool CarBase::CheckGroundPoint(int CheckColModel, VECTOR checkPos, float& outGroundY, VECTOR& outNormal)
{
    VECTOR lineStart = VAdd(checkPos, VGet(0.0f, 5.0f, 0.0f));
    VECTOR lineEnd = VAdd(checkPos, VGet(0.0f, -50.0f, 0.0f));

    MV1_COLL_RESULT_POLY HitPoly = MV1CollCheck_Line(
        CheckColModel, -1, lineStart, lineEnd
    );

    if (HitPoly.HitFlag == 1)
    {
        outGroundY = HitPoly.HitPosition.y;
        outNormal = HitPoly.Normal;
        return true;
    }

    return false;
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

MATRIX CarBase::CalculateSlopeMatrix(VECTOR normal)
{
    // 法線を正規化
    normal = VNorm(normal);

    // 上方向ベクトル(地面の法線)
    VECTOR up = normal;

    // 仮の前方向ベクトル(ワールドのZ軸方向)
    VECTOR forward = VGet(0.0f, 0.0f, 1.0f);

    // 右方向ベクトルを外積で計算
    VECTOR right = VCross(up, forward);

    // 法線が真上を向いている場合の対策
    if (VSize(right) < 0.001f) {
        // 別の基準ベクトルを使用
        forward = VGet(1.0f, 0.0f, 0.0f);
        right = VCross(up, forward);
    }

    right = VNorm(right);

    // 前方向ベクトルを再計算(右と上の外積)
    forward = VCross(right, up);
    forward = VNorm(forward);

    // 回転行列を構築
    MATRIX mat;
    mat.m[0][0] = right.x;   mat.m[0][1] = right.y;   mat.m[0][2] = right.z;   mat.m[0][3] = 0.0f;
    mat.m[1][0] = up.x;      mat.m[1][1] = up.y;      mat.m[1][2] = up.z;      mat.m[1][3] = 0.0f;
    mat.m[2][0] = forward.x; mat.m[2][1] = forward.y; mat.m[2][2] = forward.z; mat.m[2][3] = 0.0f;
    mat.m[3][0] = 0.0f;      mat.m[3][1] = 0.0f;      mat.m[3][2] = 0.0f;      mat.m[3][3] = 1.0f;

    return mat;
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

    if (hitWall && !wasHitWall) {
        justHitWall = true;
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
            //printfDx("WARNING: Large push detected: %.2f\n", pushDistance);
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
    if (justHitWall)
    {
        float currentSpeed = fabsf(moveSpeed);

        //HP減少
        float damage = currentSpeed * DAMAGE_MULTIPLIER;
        Hp -= damage;
    }
}

void CarBase::CheckCarCollision(float delta)
{
    if (allCars == nullptr) return;  // リストが設定されていない

    for (auto* otherCar : *allCars)
    {
        if (otherCar == this) continue;           // 自分自身は除外
        if (otherCar->GetHP() <= 0.0f) continue;  // 死亡した車は無視

        // 位置同士の距離で判定
        VECTOR otherPos = otherCar->GetPosition();
        VECTOR diff = VSub(otherPos, pos);
        diff.y = 0.0f;  // 水平方向のみ

        float dist = VSize(diff);

        // 衝突判定距離(お互いの半径の合計)
        float collisionDist = capsuleRadius * 2.0f;

        // 衝突している
        if (dist < collisionDist)
        {
            ProcessCarCollision(otherCar, dist);
        }
    }
}

void CarBase::ProcessCarCollision(CarBase* otherCar,float currentDist)
{
    VECTOR otherPos = otherCar->GetPosition();
    
    // 衝突方向(自分→相手)
    VECTOR toOther = VSub(otherPos, pos);
    toOther.y = 0.0f;
    float dist = VSize(toOther);
    
    if (dist < 0.01f) return;

    VECTOR collisionDir = VNorm(toOther);

    // 重なり量
    float collisionDist = capsuleRadius * 2.0f;
    float overlap = collisionDist - dist;
    if (overlap > 0.0f)
    {
        // お互いに押し出す(お互いに半分ずつ)
        VECTOR pushVec = VScale(collisionDir, overlap * 0.5f);
        pos = VSub(pos, pushVec);

        // 速度を取得
        VECTOR myVel = VGet(vel.x, 0.0f, vel.z);
        float mySpeed = VSize(myVel);

        // 相手の速度も取得
        VECTOR otherVel = otherCar->GetVelocity();
        otherVel.y = 0.0f;
        float otherSpeed = VSize(otherVel);

        // 相対速度を計算
        VECTOR relativeVel = VSub(myVel, otherVel);
        float relativeSpeed = VSize(relativeVel);

        //ダメージ計算
        float damage = relativeSpeed * CAR_COLLISION_DAMAGE_MULTIPLIER;
        if (damage > 0.1f) {  // 最低ダメージ閾値
            Hp -= damage;
            //printfDx("衝突 Damage: %.2f (RelSpeed: %.1f)\n", damage, relativeSpeed);
        }

        // 自分の進行方向
        VECTOR myDir = VGet(0.0f, 0.0f, 0.0f);
        if (mySpeed > 0.1f) {
            myDir = VNorm(myVel);
        }

        // 相手に向かっているか判定(内積)
        float myDot = VDot(myDir, collisionDir);

        // 相手が自分に向かっているか
        VECTOR otherDir = VGet(0.0f, 0.0f, 0.0f);
        if (otherSpeed > 0.1f) {
            otherDir = VNorm(otherVel);  // ← 修正: VNorm使用
        }
        float otherDot = VDot(otherDir, VScale(collisionDir, -1.0f));

        //判定
        bool MovingMe = (mySpeed > 1.0f);
        bool MovingOther = (otherSpeed > 1.0f);
        bool AttackerMe = (myDot > 0.3f);
        bool AttackerOther = (otherDot > 0.3f);

        if (MovingMe && !MovingOther)
        {
            // パターン1: 自分だけ動いている → 自分が当たった
            ReflectVelocity(collisionDir, mySpeed);
            //printfDx("Car collision: I HIT stationary car (speed: %.1f)\n", mySpeed);
        }
        else if (!MovingMe && MovingOther)
        {
            // パターン2: 相手だけ動いている → 当てられた
            GetPushed(collisionDir, otherSpeed);
            //printfDx("Car collision: I GOT HIT (pushed by: %.1f)\n", otherSpeed);
        }
        else if (MovingMe && MovingOther)
        {
            // パターン3: 両方動いている
            if (AttackerMe && !AttackerOther)
            {
                // 自分が向かっている、相手は逃げている → 自分が当たった
                ReflectVelocity(collisionDir, mySpeed);
                //printfDx("Car collision: I HIT moving car (speed: %.1f vs %.1f)\n", mySpeed, otherSpeed);
            }
            else if (!AttackerMe && AttackerOther)
            {
                // 相手が向かっている、自分は逃げている → 当てられた
                GetPushed(collisionDir, otherSpeed);
                //printfDx("Car collision: I GOT HIT by moving car\n");
            }
            else
            {
                // 正面衝突 or すれ違い → 速い方が「当たった」扱い
                if (mySpeed > otherSpeed)
                {
                    ReflectVelocity(collisionDir, mySpeed);
                    //printfDx("Car collision: HEAD-ON (I'm faster: %.1f > %.1f)\n", mySpeed, otherSpeed);
                }
                else
                {
                    GetPushed(collisionDir, otherSpeed);
                    printfDx("Car collision: HEAD-ON (I'm slower: %.1f < %.1f)\n", mySpeed, otherSpeed);
                }
            }
        }
    }
}

// 反射処理（当たった側）
void CarBase::ReflectVelocity(VECTOR collisionDir, float currentSpeed)
{
    // 現在の速度ベクトル
    VECTOR currentVel = VGet(vel.x, 0.0f, vel.z);
    VECTOR currentDir = VNorm(currentVel);

    // 反射ベクトルの計算 
    float dot = VDot(currentDir, collisionDir);
    VECTOR reflection = VScale(collisionDir, dot * 2.0f);
    VECTOR reflectDir = VSub(currentDir, reflection);
    reflectDir = VNorm(reflectDir);

    //衝突角度に応じた反発係数
    float absDot = fabsf(dot);
    float restitution;

    if (absDot > 0.7f) {
        // 正面衝突(角度が小さい) → 大きく減速
        restitution = 0.3f;
        //printfDx("  → HEAD-ON collision (dot: %.2f)\n", dot);
    }
    else if (absDot > 0.3f) {
        // 斜め衝突 → 中程度減速
        restitution = 0.7f;
        //printfDx("  → ANGLED collision (dot: %.2f)\n", dot);
    }
    else {
        // 横からの衝突 → ほぼ速度維持
        restitution = 0.95f;
        //printfDx("  → SIDE collision (dot: %.2f)\n", dot);
    }

    // 反射後の速度
    float newSpeed = currentSpeed * restitution;

    vel.x = reflectDir.x * newSpeed;
    vel.z = reflectDir.z * newSpeed;
    moveSpeed = newSpeed;

    // 角度更新
    angle = atan2f(reflectDir.x, reflectDir.z) * 180.0f / DX_PI_F;

    //printfDx("  → Reflected: %.1f -> %.1f (restitution: %.2f)\n",
    //    currentSpeed, newSpeed, restitution);
}



// 押し出し処理(当てられた側)
void CarBase::GetPushed(VECTOR collisionDir, float pusherSpeed)
{
    // 現在の速度ベクトル
    VECTOR currentVel = VGet(vel.x, 0.0f, vel.z);
    VECTOR currentDir = VGet(0.0f, 0.0f, 0.0f);
    float currentSpeed = VSize(currentVel);

    if (currentSpeed > 0.1f) {
        currentDir = VNorm(currentVel);
    }


    // 衝突方向と逆
    VECTOR pushDir = VScale(collisionDir, -1.0f);

    //衝突角度に応じた処理
    float dot = VDot(currentDir, pushDir);

    if (dot < -0.5f) {
        // 追突された(同じ方向に走っていた)
        // → 前の車の速度を受け継ぐ
        float speedDiff = pusherSpeed - currentSpeed;
        if (speedDiff > 0.0f) {
            // 加速される
            VECTOR addVel = VScale(pushDir, speedDiff * 0.8f);
            VECTOR newVel = VAdd(currentVel, addVel);

            vel.x = newVel.x;
            vel.z = newVel.z;
            moveSpeed = VSize(newVel);

            //printfDx("  → Pushed FORWARD: %.1f -> %.1f\n", currentSpeed, moveSpeed);
        }
        else {
            // 自分の方が速い → そのまま
            //printfDx("  → Already faster, no push\n");
        }
    }
    else {
        // 横/斜めから押された
        // → 押される力を加算
        float pushForce = pusherSpeed * 0.5f;
        VECTOR pushVel = VScale(pushDir, pushForce);
        VECTOR newVel = VAdd(currentVel, pushVel);

        vel.x = newVel.x;
        vel.z = newVel.z;
        moveSpeed = VSize(newVel);

        //printfDx("  → Pushed SIDE: %.1f + %.1f = %.1f\n",
        //    currentSpeed, pushForce, moveSpeed);
    }

    // 角度更新(新しい速度方向を向く)
    if (moveSpeed > 0.1f) {
        angle = atan2f(vel.x, vel.z) * 180.0f / DX_PI_F;
    }
}