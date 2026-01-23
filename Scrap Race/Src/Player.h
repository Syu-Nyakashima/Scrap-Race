#pragma once
#include "CarBase.h"

class Player : public CarBase
{
public:
    Player(Stage& stageRef);
    ~Player() override;

    //ゲーム内の初期化
    void Initialize() override;
    void Terminate() override;


    void Update(float delta) override;
    void Draw() override;

    // Getter関数
    VECTOR GetPos() const { return pos; }
    float GetAngle() const { return angle; }
    float GetMoveSpeed() const { return moveSpeed; }
    float GetHp() const { return Hp; }

private:
    // 入力処理
    void HandleRotation(float delta);       // 回転入力
    void HandleAcceleration(float delta);   // 加速・減速入力
    void HandleDrift(float delta);

protected:
    // 更新
    void UpdateInput(float delta) override;          // 入力処理

private:
    // 通常旋回は遅く
    const float NORMAL_ROTATION_SPEED = 90.0f;  // 通常時の旋回速度（遅い）

    // ドリフト時のパラメータ
    const float DRIFT_ROTATION_BOOST = 1.0f;    // ドリフト時の旋回速度倍率
    const float DRIFT_ANGLE_SPEED = 30.0f;     // ドリフト角度の蓄積速度
    const float DRIFT_RECOVERY = 0.90f;         // ドリフト終了時の復帰速度
    const float DRIFT_LATERAL_MULT = 1.0f;      // 横滑り量
};