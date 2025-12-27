#pragma once
#include "DxLib.h"
#include "Stage.h"

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

    // ゲッター
    VECTOR GetPosition() const { return pos; }
    float GetAngle() const { return angle; }
    float GetHP() const { return Hp; }
    bool IsAlive() const { return Hp > 0.0f; }

protected:
    // 共通の物理演算・衝突判定
    void UpdatePhysics(float delta);
    void UpdateCollision(float delta);

    void CheckGround(int CheckColModel, float delta);
    bool CheckGroundPoint(int CheckColModel, VECTOR checkPos, float& outGroundY);
    void ApplyGroundPhysics(float groundY, float delta);

    void CheckWall(int CheckColModel, float delta);
    void ProcessWallCollision(const MV1_COLL_RESULT_POLY_DIM& HitPolyDim);
    void ApplyWallPushOut(VECTOR pushOut, VECTOR avgNormal);
    void ApplyWallDamage();

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

    // 当たり判定パラメータ
    float capsuleRadius = 5.0f;
    float capsuleHeight = 3.0f;

    // 状態フラグ
    bool hitWall;
    bool wasHitWall;
    bool onGround;

protected:
    // ステージ情報
    Stage& stage;
    int ModelHandle;

    // 物理定数
    static constexpr float GRAVITY = -9.8f;
    static constexpr float ROTATION_SPEED = 180.0f;
    static constexpr float RESTITUTION = 0.3f;
    static constexpr float HP_DRAIN_PER_FRAME = 0.01f;
    static constexpr float DAMAGE_MULTIPLIER = 0.1f;
    static constexpr float FOOT_OFFSET_RATIO = 0.5f;
    static constexpr int MAX_WALL_ITERATIONS = 3;

    static constexpr float WALL_NORMAL_THRESHOLD = 0.7f;
};