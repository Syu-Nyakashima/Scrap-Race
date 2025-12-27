#include "EnemyCPU.h"

EnemyCPU::EnemyCPU(Stage& stageRef, AIDifficulty diff/*, AIType type*/)
	: CarBase(stageRef), difficulty(diff),/* type(type),*/
    currentWaypointIndex(0), thinkTimer(0.0f), stuckTimer(0.0f),
    isStuck(false), hasNearestScrap(false), scrapSearchRadius(SCRAP_SEARCH_RADIUS),
    errorTimer(0.0f), isInError(false)
{
    // 難易度別パラメータ設定
    switch (difficulty)
    {
    case AIDifficulty::Easy:
        reactionTime = 0.3f;
        corneringSkill = 0.6f;
        recoverySpeed = 0.5f;
        errorRate = 0.3f;
        errorDuration = 1.5f;
        SpdMax = 120.0f;
        break;

    case AIDifficulty::Normal:
        reactionTime = 0.15f;
        corneringSkill = 0.8f;
        recoverySpeed = 0.8f;
        errorRate = 0.15f;
        errorDuration = 0.8f;
        SpdMax = 150.0f;
        break;

    case AIDifficulty::Hard:
        reactionTime = 0.05f;
        corneringSkill = 0.95f;
        recoverySpeed = 1.0f;
        errorRate = 0.05f;
        errorDuration = 0.3f;
        SpdMax = 180.0f;
        break;
    }

    // 性格別パラメータ設定
    SetTypeParameters();
}

EnemyCPU::~EnemyCPU()
{
    Terminate();
}

void EnemyCPU::Initialize()
{
    CarBase::Initialize();

    //ModelHandle = MV1LoadModel();
    if (ModelHandle == -1) {
        printfDx("敵モデル読み込み失敗！プレイヤーモデル使用\n");
        ModelHandle = MV1LoadModel("Data/Model/RaceCarModel.mv1");
    }

    currentWaypointIndex = 0;
    thinkTimer = 0.0f;
    stuckTimer = 0.0f;
    isStuck = false;
    lastPos = pos;
    hasNearestScrap = false;
    errorTimer = 0.0f;
    isInError = false;
}

void EnemyCPU::Terminate()
{
    CarBase::Terminate();
}

void EnemyCPU::Update(float delta)
{
    // HP自然減少
    Hp -= HP_DRAIN_PER_FRAME;
    if (Hp <= 0.0f) {
        Hp = 0.0f;
        return; // 死亡時は動かない
    }

    //ステータス減少
    //DrainStatusOverTime(delta);

    // AI思考
    ThinkAI(delta);

    //エラー挙動
    //UpdateErrorBehavior(delta);

    // 入力処理(AIが決定)
    UpdateInput(delta);

    // 物理演算
    UpdatePhysics(delta);

    // 衝突判定
    UpdateCollision(delta);

    // スタック検出
    CheckStuckState(delta);
}

void EnemyCPU::SetTypeParameters()
{
}

void EnemyCPU::ThinkAI(float delta)
{
    thinkTimer += delta;

    if (thinkTimer >= THINK_INTERVAL)
    {
        thinkTimer = 0.0f;

        // ウェイポイント更新
        UpdateWaypointNavigation();
    }
}

void EnemyCPU::UpdateWaypointNavigation()
{
    if (waypoints.empty()) return;

    VECTOR currentWaypoint = waypoints[currentWaypointIndex];
    float distToWaypoint = VSize(VSub(currentWaypoint, pos));

    // ウェイポイント到達判定
    if (distToWaypoint < WAYPOINT_RADIUS)
    {
        currentWaypointIndex = (currentWaypointIndex + 1) % waypoints.size();
    }
}

void EnemyCPU::UpdateInput(float delta)
{
    //スタックしていたら
    if (isStuck)
    {
        RecoverFromStuck(delta);
        return;
    }

    // 目標地点を取得
    VECTOR target = GetCurrentTarget();

    // ステアリング
    SteerToTarget(target, delta);

    // アクセル制御
    HandleAcceleration(delta);
}

void EnemyCPU::RecoverFromStuck(float delta)
{
    // バック&ターン
    moveSpeed = -SpdMax * 0.3f;
    angle += ROTATION_SPEED * delta;

    stuckTimer -= delta * 2.0f; // 回復

    if (stuckTimer <= 0.0f)
    {
        isStuck = false;
        stuckTimer = 0.0f;
    }
}

VECTOR EnemyCPU::GetCurrentTarget() const
{
    // HP低下時はスクラップ優先
    if (ShouldSearchScrap() && hasNearestScrap)
    {
        //性格による優先度判定
        //float randValue = (float)rand() / RAND_MAX;
        //if(randValue < scrapPriority)
        //{
            return nearestScrapPos;
        //}
    }

    // 通常はウェイポイント
    if (!waypoints.empty())
    {
        return waypoints[currentWaypointIndex];
    }

    // ウェイポイントがない場合は前方
    float rad = angle * DX_PI_F / 180.0f;
    return VAdd(pos, VGet(sinf(rad) * 10.0f, 0.0f, cosf(rad) * 10.0f));
}

bool EnemyCPU::ShouldSearchScrap() const
{
    return Hp < LOW_HP_THRESHOLD;
}

void EnemyCPU::SteerToTarget(VECTOR targetPos, float delta)
{
    float targetAngle = GetAngleToTarget(targetPos);
    float angleDiff = targetAngle - angle;

    // 角度を-180~180に正規化
    while (angleDiff > 180.0f) angleDiff -= 360.0f;
    while (angleDiff < -180.0f) angleDiff += 360.0f;

    // コーナリングスキルを反映
    float steerAmount = angleDiff * corneringSkill;

    // エラー状態ならハンドル操作がおかしくなる
    /*
    if (isInError)
    {
        // ランダムに左右に振れる
        float errorSteer = ((float)rand() / RAND_MAX - 0.5f) * 90.0f;
        steerAmount += errorSteer;
    }
    */

    steerAmount = fmaxf(-ROTATION_SPEED * delta, fminf(ROTATION_SPEED * delta, steerAmount * delta));

    angle += steerAmount;
}

float EnemyCPU::GetAngleToTarget(VECTOR target) const
{
    VECTOR toTarget = VSub(target, pos);
    float targetAngle = atan2f(toTarget.x, toTarget.z) * 180.0f / DX_PI_F;
    return targetAngle;
}

void EnemyCPU::HandleAcceleration(float delta)
{
    float targetSpeed = CalculateTargetSpeed();

    //エラー状態
    

    // 目標速度に向けて加減速
    if (moveSpeed < targetSpeed)
    {
        moveSpeed += SpdUp;
        if (moveSpeed > targetSpeed) moveSpeed = targetSpeed;
    }
    else if (moveSpeed > targetSpeed)
    {
        moveSpeed -= SpdDown;
        if (moveSpeed < targetSpeed) moveSpeed = targetSpeed;
    }

    // 壁衝突時はブレーキ
    if (hitWall)
    {
        moveSpeed *= 0.5f;
    }
}

float EnemyCPU::CalculateTargetSpeed() const
{
    VECTOR target = GetCurrentTarget();
    float angleToTarget = GetAngleToTarget(target);
    float angleDiff = fabsf(angleToTarget - angle);

    // 角度差が大きいほど減速
    if (angleDiff > 45.0f)
    {
        return SpdMax * 0.5f;
    }
    else if (angleDiff > 20.0f)
    {
        return SpdMax * 0.7f;
    }

    return SpdMax;
}

void EnemyCPU::CheckStuckState(float delta)
{
    float movedDistance = VSize(VSub(pos, lastPos));

    //動いた距離が一定時間に一定距離動いていなければ
    if (movedDistance < STUCK_THRESHOLD * delta && fabsf(moveSpeed) > 1.0f)
    {
        stuckTimer += delta;

        if (stuckTimer > STUCK_TIME)
        {
            isStuck = true;
        }
    }
    else
    {
        stuckTimer = 0.0f;
        isStuck = false;
    }

    lastPos = pos;
}

void EnemyCPU::SetWaypoints(const std::vector<VECTOR>& points)
{
    waypoints = points;
    currentWaypointIndex = 0;
}

void EnemyCPU::SetNearestScrap(VECTOR scrapPos)
{
    nearestScrapPos = scrapPos;
    hasNearestScrap = true;
}

void EnemyCPU::ClearNearestScrap()
{
    hasNearestScrap = false;
}

void EnemyCPU::Draw()
{
    CarBase::Draw();
}