#pragma once
#include "DxLib.h"
#include "Stage.h"

class Player
{
public:
    //依存関係の注入
    Player(Stage& stageRef);

    //リソース解放
    ~Player();

    //ゲーム内の初期化
    void Initialize();

    void Update(float delta);
    void Draw();
    void Heal(float amount);

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

    //速度
    VECTOR vel;
    float SpdUp;
    float SpdDown;
    float SpdMax;
    float SpdMin;

    const float GRAVITY = -9.8f;
    bool onGround = false;
    bool wasHitWall = false;

    struct Transform {
        VECTOR position;
        VECTOR rotation;
        VECTOR scale;
        MATRIX world;
    } transform;

    Stage& stage;  // 参照にしてStage側と同じモデルを使用できるように

    VECTOR SpherePos;//球判定

    void CheckGround(int CheckColModel, float delta);//線分使用
    void CheckWall(int CheckColModel, float delta);//球判定使用
};