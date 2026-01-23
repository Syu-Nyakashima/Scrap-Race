#pragma once
#include "DxLib.h"
#include "Stage.h"
#include <vector>

class ItemManager;

class CarBase {
public:
    CarBase(Stage& stageRef);
    virtual ~CarBase();

    virtual void Initialize();
    virtual void Terminate();
    virtual void Update(float delta) = 0; // 純粋仮想関数
    virtual void Draw();

    // 回復処理
    void Heal(float amount);
    void BoostStatus(float spdMaxBoost, float spdUpBoost);

    // ItemManagerの設定
    void SetItemManager(ItemManager* itemMgr) { itemManager = itemMgr; }

    // ゲッター
    VECTOR GetPosition() const { return pos; }
    float GetAngle() const { return angle; }
    VECTOR GetVelocity() const { return vel; }
    float GetHP() const { return Hp; }
    bool IsAlive() const { return Hp > 0.0f; }
    bool IsGoaled() const { return isGoal; }
    bool JustHitWall() const { return justHitWall; }
    bool GetHitWall() const { return hitWall; } 

    void SetCarList(std::vector<CarBase*>* carList) { allCars = carList; }

protected:
    //ステータス変動
    void DrainStatusOverTime(float delta);

    // HP減少によるスクラップ生成チェック
    void CheckHPDropScrap();

    // 共通の物理演算・衝突判定
    void UpdatePhysics(float delta);
    void UpdateCollision(float delta);

    void CheckGround(int CheckColModel, float delta);
    bool CheckGroundPoint(int CheckColModel, VECTOR checkPos, float& outGroundY, VECTOR& outNormal);
    void ApplyGroundPhysics(float groundY, float delta);
    MATRIX CalculateSlopeMatrix(VECTOR normal);

    void CheckWall(int CheckColModel, float delta);
    void ProcessWallCollision(const MV1_COLL_RESULT_POLY_DIM& HitPolyDim);
    void ApplyWallPushOut(VECTOR pushOut, VECTOR avgNormal);
    void ApplyWallDamage();

    void CheckCarCollision( float delta);
    void ProcessCarCollision(CarBase* otherCar, float currentDist);
    void ReflectVelocity(VECTOR collisionDir, float currentSpeed);
    void GetPushed(VECTOR collisionDir, float pusherSpeed);
	void SpawnCarCollisionScrap(VECTOR collisionPos, float relativeSpeed);

    // 既存のメンバー
    std::vector<CarBase*>* allCars;  // 全車両へのポインタ
    ItemManager* itemManager;

    // サブクラスで実装する入力処理
    virtual void UpdateInput(float delta) = 0;

public:
    // 位置・速度
    VECTOR pos;
    VECTOR vel;
    VECTOR SpherePos;
    float angle;
    float moveSpeed;

    // ステータス
    float SpdMax;
    float SpdMin;
    float SpdUp;
    float SpdDown;
    float Hp;

    // ステータス下限
    static constexpr float MIN_SPD_MAX = 100.0f;

    // ステータス上限
    static constexpr float MAX_SPD_MAX = 200.0f;
    static constexpr float MAX_SPD_UP = 2.0f;


    // ステータス減少率
    static constexpr float STATUS_DRAIN_SPD_MAX = 1.0f;
    static constexpr float STATUS_DRAIN_SPD_UP = 0.1f;

    // 当たり判定パラメータ
    float capsuleRadius;
    float capsuleHeight;

    // 状態フラグ
    bool hitWall;
    bool wasHitWall;
    bool justHitWall;
    bool onGround;
    bool isGoal;

    //ラップ周回
    int currentLap;
    int currentCheckpoint;

protected:
    // ステージ情報
    Stage& stage;
    int ModelHandle;

    bool isDrifting;
    float driftAngle;           // ドリフトによる横向き角度
    float lateralVelocity;      // 横方向の速度

    VECTOR groundNormal;  // 地面の法線ベクトル

    // HP減少による生成管理
    float hpDrainSum;             // HP減少の蓄積量
    float lastHP;

    // 定数
    const float HP_DROP_THRESHOLD = 10.0f; // HPがこの値減ったらRareScrapを出現させる
    static constexpr float CAR_COLLISION_SCRAP_SPEED_THRESHOLD = 30.0f; // 車衝突でスクラップ生成する相対速度閾値
    
    // 物理定数
    static constexpr float GRAVITY = -9.8f;
    static constexpr float ROTATION_SPEED = 180.0f;
    static constexpr float RESTITUTION = 0.3f;
    static constexpr float HP_DRAIN_PER_FRAME = 0.02f;
    static constexpr float DAMAGE_MULTIPLIER = 0.1f;
    static constexpr float FOOT_OFFSET_RATIO = 0.5f;
    static constexpr int MAX_WALL_ITERATIONS = 3;
    static constexpr float WALL_NORMAL_THRESHOLD = 0.7f;
    static constexpr float CAR_COLLISION_RESTITUTION = 0.8f;  // ← 修正: Car衝突の基本反発係数
    static constexpr float CAR_COLLISION_DAMAGE_MULTIPLIER = 5.0f;  // ← 修正: Car衝突のダメージ倍率
};