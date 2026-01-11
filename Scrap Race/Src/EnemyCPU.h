#pragma once
#include "CarBase.h"
#include <vector>

// AI難易度
enum class AIDifficulty {
	Easy,
	Normal,
	Hard
};

enum class AIType {
    Attack,         // 攻撃
    Defense,        // 安全
    Balance,        // バランス
    obstruction,    // 妨害
    ScrapHunter     // スクラップ優先
};

class EnemyCPU : public CarBase
{
public:
	EnemyCPU(Stage& stageRef, AIDifficulty diff, AIType type);
	~EnemyCPU() override;

	void Initialize() override;
    void Terminate() override;
	void Update(float delta) override;
    void Draw() override;

	void SetWaypoints(const std::vector<VECTOR>& points);
	void SetNearestScrap(VECTOR scrapPos);
	void ClearNearestScrap();

protected:
    //入力処理（AI）
    void UpdateInput(float delta) override;

private:
    // 性格別パラメータ設定
    void SetTypeParameters();

    // AI思考
    void ThinkAI(float delta);
    void UpdateWaypointNavigation();

    //入力処理
    void SteerToTarget(VECTOR targetPos, float delta);
    void HandleAcceleration(float delta);
    float CalculateTargetSpeed() const;
    
    //スタック検出
    void CheckStuckState(float delta);

    //回復処理
    void RecoverFromStuck(float delta);
    void OnWallHit();
    void RecoverFromWallHit(float delta);
    
    // 目標設定
    bool ShouldSearchScrap() const;
    VECTOR GetCurrentTarget() const;
    float GetAngleToTarget(VECTOR target) const;

    //エラー挙動
    void UpdateErrorBehavior(float delta);
    void TriggerRandomError();

private:
    // AI設定
    AIDifficulty difficulty;
    AIType type;

    // AI思考パラメータ
    float reactionTime;          // 反応速度
    float corneringSkill;        // コーナリング技術(0.0~1.0)
    float recoverySpeed;         // 立て直し速度
    float aggressiveness;        // 攻撃性(壁を気にしない度合い)
    float scrapPriority;         // スクラップ優先度(0.0~1.0)

    // エラー挙動パラメータ
    float errorRate;             // ミス発生率(0.0~1.0)
    float errorTimer;            // エラー状態タイマー
    float errorDuration;         // エラー継続時間
    bool isInError;              // エラー状態中

    // ウェイポイント(コース上の目標地点)
    std::vector<VECTOR> waypoints;
    int currentWaypointIndex;
    float lastWaypointDist;

    // AI状態
    float thinkTimer;            // 思考更新タイマー
    float stuckTimer;            // スタック検出タイマー
    VECTOR lastPos;              // 前フレームの位置
    bool isStuck;                // スタック状態

    // スクラップ探索
    VECTOR nearestScrapPos;
    bool hasNearestScrap;
    float scrapSearchRadius;

    // 壁衝突回復
    float wallHitRecoveryTimer;

    // 定数
    static constexpr float THINK_INTERVAL = 0.1f;      // 思考更新間隔
    static constexpr float WAYPOINT_RADIUS = 40.0f;     // ウェイポイント到達判定
    static constexpr float STUCK_THRESHOLD = 1.0f;     // スタック判定距離
    static constexpr float STUCK_TIME = 2.0f;          // スタック判定時間
    static constexpr float LOW_HP_THRESHOLD = 30.0f;   // HP低下判定
    static constexpr float SCRAP_SEARCH_RADIUS = 20.0f;// スクラップを探す範囲
    static constexpr float ERROR_CHECK_INTERVAL = 2.0f;  // エラー判定間隔
    static constexpr float WALL_RECOVERY_TIME = 2.0f;  // 壁衝突時の回復時間
};