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

protected:
    // 更新
    void UpdateInput(float delta) override;          // 入力処理
};