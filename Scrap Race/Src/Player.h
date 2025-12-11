#pragma once
#include "DxLib.h"
#include "Stage.h"

class Player
{
public:
    Player(Stage& stageRef);
    ~Player();

    //ゲーム内の初期化
    void Initialize();
    void Terminate();
    void Update(float delta);
    void Draw();

    void Heal(float amount);

    // Getter関数
    VECTOR GetPos() const { return pos; }
    float GetAngle() const { return angle; }
    float GetMoveSpeed() const { return moveSpeed; }
    float GetHp() const { return Hp; }

private:
    // 更新
    void UpdateInput(float delta);          // 入力処理
    void UpdatePhysics(float delta);        // 物理計算
    void UpdateCollision(float delta);      // 当たり判定

    // 入力処理
    void HandleRotation(float delta);       // 回転入力
    void HandleAcceleration(float delta);   // 加速・減速入力

    //当たり判定
    void CheckGround(int CheckColModel, float delta);// 線分使用
    void CheckWall(int CheckColModel, float delta);  // 球判定使用

    // 地面判定の内部処理
    bool CheckGroundPoint(int CheckColModel, VECTOR checkPos, float& outGroundY);
    void ApplyGroundPhysics(float groundY, float delta);

    // 壁判定の内部処理
    void ProcessWallCollision(const MV1_COLL_RESULT_POLY_DIM& HitPolyDim);
    void ApplyWallPushOut(VECTOR pushOut, VECTOR avgNormal);
    void ApplyWallDamage();

public:
    VECTOR pos;
    float radius = 5.0f;
    float Hp = 100.0f;

    //ImGui用にpublicに、あとでprivateに戻す
    float angle;
    float moveSpeed;
    float capsuleRadius = 1.5f;  // 横幅
    float capsuleHeight = 2.0f;  // 高さ
    //

    bool hitWall = false;

private:
    int ModelHandle;

    // 速度
    VECTOR vel;
    float SpdUp;
    float SpdDown;
    float SpdMax;
    float SpdMin;

    bool onGround = false;
    bool wasHitWall = false;

    struct Transform {
        VECTOR position;
        VECTOR rotation;
        VECTOR scale;
        MATRIX world;
    } transform;

    Stage& stage;  // 参照にしてStage側と同じモデルを使用できるように

    VECTOR SpherePos;// 球判定

    // 定数
    static constexpr float GRAVITY = -9.8f;
    static constexpr float ROTATION_SPEED = 180.0f;
    static constexpr float FOOT_OFFSET_RATIO = 0.5f;
    static constexpr float WALL_NORMAL_THRESHOLD = 0.7f;
    static constexpr float RESTITUTION = 0.3f;
    static constexpr float DAMAGE_MULTIPLIER = 0.1f;
    static constexpr float HP_DRAIN_PER_FRAME = 0.01f;
    static constexpr int MAX_WALL_ITERATIONS = 3;
};