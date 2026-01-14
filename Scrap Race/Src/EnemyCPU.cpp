#include "EnemyCPU.h"

EnemyCPU::EnemyCPU(Stage& stageRef, AIDifficulty diff, AIType type)
    : CarBase(stageRef), difficulty(diff), type(type),
    currentWaypointIndex(0), thinkTimer(0.0f), stuckTimer(0.0f),
    isStuck(false), hasNearestScrap(false), scrapSearchRadius(SCRAP_SEARCH_RADIUS),
    errorTimer(0.0f), isInError(false), wallHitRecoveryTimer(0.0f),
    consecutiveWallHits(0), wallHitCooldown(0.0f), lastWallHitTime(0.0f)
{   
    // 難易度別パラメータ設定
    switch (difficulty)
    {
    case AIDifficulty::Easy:
        reactionTime = 0.3f;
        corneringSkill = 0.6f;
        recoverySpeed = 0.5f;
        errorRate = 0.3f;
        errorDuration = 3.0f;
        SpdMax = 120.0f;
        break;

    case AIDifficulty::Normal:
        reactionTime = 0.15f;
        corneringSkill = 0.8f;
        recoverySpeed = 0.8f;
        errorRate = 0.15f;
            errorDuration = 2.0f;
            SpdMax = 150.0f;
            break;

    case AIDifficulty::Hard:
        reactionTime = 0.05f;
        corneringSkill = 0.95f;
        recoverySpeed = 1.0f;
        errorRate = 0.05f;
        errorDuration = 1.0f;
        SpdMax = 180.0f;
        break;
    }

    // 性格別パラメータ設定
    SetTypeParameters();
}

EnemyCPU::~EnemyCPU()
{
}

void EnemyCPU::Initialize()
{
    CarBase::Initialize();  

    //ModelHandle = MV1LoadModel();
    if (ModelHandle == -1) {
        //printfDx("敵モデル読み込み失敗！プレイヤーモデル使用\n");
        ModelHandle = MV1LoadModel("Data/Model/free_car_1.mv1");
    }

    currentWaypointIndex = 0;
    thinkTimer = 0.0f;
    stuckTimer = 0.0f;
    isStuck = false;
    lastPos = pos;
    hasNearestScrap = false;
    errorTimer = 0.0f;
    isInError = false;
    wallHitRecoveryTimer = 0.0f;
    lastWaypointDist = 99999.0f;
    consecutiveWallHits = 0;
    wallHitCooldown = 0.0f;
    lastWallHitTime = 0.0f;
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
    DrainStatusOverTime(delta);

    // AI思考
    ThinkAI(delta);

    //エラー挙動
    UpdateErrorBehavior(delta);

    // 壁衝突クールダウン更新
    if (wallHitCooldown > 0.0f) {
        wallHitCooldown -= delta;
    }

    // 壁衝突回復タイマー更新
    if (wallHitRecoveryTimer > 0.0f) {
        wallHitRecoveryTimer -= delta;
    }

    // 入力処理(AIが決定)
    UpdateInput(delta);

    // 物理演算
    UpdatePhysics(delta);

    // 衝突判定
    UpdateCollision(delta);

    // 壁衝突検出 
    if (justHitWall) {
        OnWallHit();
    }

    // スタック検出
    CheckStuckState(delta);

    // 連続衝突カウントの減衰(2秒間衝突がなければリセット)
    lastWallHitTime += delta;
    if (lastWallHitTime > 2.0f && consecutiveWallHits > 0) {
        consecutiveWallHits -= delta * 0.5f;
        if (consecutiveWallHits < 0) consecutiveWallHits = 0;
    }
}

void EnemyCPU::SetTypeParameters()
{
    switch (type)
    {
    case AIType::Attack:
        aggressiveness = 1.0f;      // 壁を気にしない
        scrapPriority = 0.3f;       // スクラップ優先度低
        corneringSkill *= 0.8f;     // コーナリング下手
        break;

    case AIType::Defense:
        aggressiveness = 0.5f;      // 壁を避ける
        scrapPriority = 0.5f;       // スクラップ優先度中
        corneringSkill *= 1.2f;     // コーナリング上手
        break;

    case AIType::Balance:
        aggressiveness = 0.75f;
        scrapPriority = 0.5f;
        // デフォルト値のまま
        break;

    case AIType::ScrapHunter:
        aggressiveness = 0.7f;
        scrapPriority = 0.9f;       // スクラップ最優先
        scrapSearchRadius = 30.0f;  // 探索範囲拡大
        break;
    }
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
    if (distToWaypoint < WAYPOINT_RADIUS) // ウェイポイント到達
    {
        currentWaypointIndex = (currentWaypointIndex + 1) % waypoints.size();
        lastWaypointDist = 99999.0f;
        //printfDx("Enemy reached waypoint %d\n", currentWaypointIndex);
        return;
    }
    
    if (distToWaypoint > lastWaypointDist + 1.0f) // 逆走検出
    {
        currentWaypointIndex = (currentWaypointIndex + 1) % waypoints.size();
       //printfDx("Enemyウェイポイントスキップ %d (moving away: %.1f -> %.1f)\n",
       //     currentWaypointIndex, lastWaypointDist, distToWaypoint);
        lastWaypointDist = 99999.0f;  // リセット
    }
    else
    {
        // 距離を更新
        lastWaypointDist = distToWaypoint;
    }
}

void EnemyCPU::UpdateInput(float delta)
{
    //壁に衝突したら
    if (wallHitRecoveryTimer > 0.0f)
    {
        RecoverFromWallHit(delta);
        return;
    }

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
        consecutiveWallHits = 0;
    }
}

void EnemyCPU::OnWallHit()
{
    // クールダウン中は無視(連続衝突防止)
    if (wallHitCooldown > 0.0f) {
        return;
    }

    consecutiveWallHits += 1.0f;
    wallHitCooldown = 0.8f;  // 0.8秒のクールダウン
    lastWallHitTime = 0.0f;  // リセット

    // 連続衝突レベルに応じた対応
    if (consecutiveWallHits >= 4.0f) {
        // レベル3: かなり危険 - 長めのバック
        wallHitRecoveryTimer = 2.5f;
        moveSpeed = -SpdMax * 0.4f;
        //printfDx("Enemy: 重度の連続衝突! 長めのバック (count: %.0f)\n", consecutiveWallHits);
    }
    else if (consecutiveWallHits >= 2.0f) {
        // レベル2: 危険 - バック
        wallHitRecoveryTimer = 1.5f;
        moveSpeed = -SpdMax * 0.3f;
        //printfDx("Enemy: 連続衝突検出! バック開始 (count: %.0f)\n", consecutiveWallHits);
    }
    else {
        // レベル1: 通常の衝突 - 停止して向き直し
        wallHitRecoveryTimer = 1.0f;
        moveSpeed = 0.0f;
        //printfDx("Enemy: 壁衝突 (count: %.0f)\n", consecutiveWallHits);
    }
}

void EnemyCPU::RecoverFromWallHit(float delta)
{
    // 目標に向き直る
    VECTOR target = GetCurrentTarget();
    float targetAngle = GetAngleToTarget(target);
    float angleDiff = targetAngle - angle;

    // 角度を-180~180に正規化
    while (angleDiff > 180.0f) angleDiff -= 360.0f;
    while (angleDiff < -180.0f) angleDiff += 360.0f;

    // 連続衝突レベルに応じた回転速度
    float turnSpeed = ROTATION_SPEED * delta;
    if (consecutiveWallHits >= 4.0f) {
        turnSpeed *= 2.5f;  // より速く回転
    }
    else if (consecutiveWallHits >= 2.0f) {
        turnSpeed *= 1.8f;
    }

    // 向き直り中は回転のみ
    if (fabsf(angleDiff) > 20.0f)
    {
        float steerAmount = angleDiff > 0 ? ROTATION_SPEED * delta : -ROTATION_SPEED * delta;
        angle += steerAmount;
        // 連続衝突が多い場合はバック継続
        if (consecutiveWallHits >= 2.0f) {
            float backSpeed = -SpdMax * 0.3f;
            if (consecutiveWallHits >= 4.0f) {
                backSpeed = -SpdMax * 0.4f;
            }
            moveSpeed = backSpeed;
        }
        else {
            moveSpeed = 0.0f;
        }
    }
    else
    {
        // 向き直ったら前進再開
        moveSpeed += SpdUp;  // ゆっくり加速
        if (moveSpeed > SpdMax) moveSpeed = SpdMax;
    }
}

VECTOR EnemyCPU::GetCurrentTarget() const
{
    // HP低下時はスクラップ優先
    if (ShouldSearchScrap() && hasNearestScrap)
    {
        //性格による優先度判定
        float randValue = (float)rand() / RAND_MAX;
        if(randValue < scrapPriority)
        {
          return nearestScrapPos;
        }
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
    if (isInError)
    {
        // ランダムに左右に振れる
        float errorSteer = ((float)rand() / RAND_MAX - 0.5f) * 90.0f;
        steerAmount += errorSteer;
    }

    steerAmount = fmaxf(-ROTATION_SPEED * delta, fminf(ROTATION_SPEED * delta, steerAmount * delta));

    angle += steerAmount;
}

float EnemyCPU::GetAngleToTarget(VECTOR target) const
{
    VECTOR toTarget = VSub(target, pos);
    float targetAngle = atan2f(toTarget.x, toTarget.z) * 180.0f / DX_PI_F;
    return targetAngle;
}

void EnemyCPU::UpdateErrorBehavior(float delta)
{
    // エラー状態中の処理
    if (isInError)
    {
        errorTimer -= delta;
        if (errorTimer <= 0.0f)
        {
            isInError = false;
        }
        return;
    }

    // 定期的にエラー判定
    errorTimer += delta;
    if (errorTimer >= ERROR_CHECK_INTERVAL)
    {
        errorTimer = 0.0f;
        TriggerRandomError();
    }
}

void EnemyCPU::TriggerRandomError()
{
    float randValue = (float)rand() / RAND_MAX;

    // 難易度によってエラー挙動のしやすさを変える
    if (randValue < errorRate)
    {
        isInError = true;
        errorTimer = errorDuration;

       // printfDx("Enemy Error! (%.1f秒)\n", errorDuration);
    }
}

void EnemyCPU::HandleAcceleration(float delta)
{
    float targetSpeed = CalculateTargetSpeed();

    //エラー状態
    if (isInError)
    {
        // アクセルとブレーキを間違える
        targetSpeed *= 0.5f + ((float)rand() / RAND_MAX) * 0.5f;
    }

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
}

float EnemyCPU::CalculateTargetSpeed() const
{
    VECTOR target = GetCurrentTarget();
    float angleToTarget = GetAngleToTarget(target);
    float angleDiff = fabsf(angleToTarget - angle);

    // 角度を0~180に正規化
    while (angleDiff > 180.0f) angleDiff -= 360.0f;
    angleDiff = fabsf(angleDiff);

    // 常に最高速を目指す(aggressiveness補正を削除)
    float baseSpeed = SpdMax;

    // コーナリングスキルによって減速度を調整
    float skillFactor = corneringSkill;

    // 角度差による減速(スキルで補正)
    if (angleDiff > 90.0f)
    {
        // 大カーブ
        return baseSpeed * (0.6f + 0.25f * skillFactor);
    }
    else if (angleDiff > 45.0f)
    {
        // 中カーブ
        return baseSpeed * (0.8f + 0.15f * skillFactor);
    }

    // 直線やゆるいカーブ: 常に最高速
    return baseSpeed;
}

void EnemyCPU::CheckStuckState(float delta)
{
    float movedDistance = VSize(VSub(pos, lastPos));

    // 連続衝突が非常に多い場合は強制スタック
    if (consecutiveWallHits >= 6.0f) {
        isStuck = true;
        stuckTimer = STUCK_TIME;
        consecutiveWallHits = 0;  // リセット
        //printfDx("Enemy: 連続衝突多数で強制スタック判定\n");
    }

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
        if (stuckTimer > 0.0f) {
            stuckTimer -= delta * 0.5f;  // 徐々に減らす
        }
        if (movedDistance > STUCK_THRESHOLD * delta * 2.0f) {
            isStuck = false;
        }
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