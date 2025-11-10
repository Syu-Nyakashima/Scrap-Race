#pragma once
#include "Player.h"
#include "Camera.h"
#include "Stage.h"
#include "ItemManager.h"

class PlayScene
{
public:
    void Initialize();
    void Terminate();

    void Update(); // ループ内の処理を書く関数
private:
    void InitImGui();
    void TerminateImGui();
    void DrawPlayerDebugUI();

    float deltaTime = 0.0f;   // 1フレームの経過時間
    float totalTime = 0.0f;   // プレイ開始からの累計時間
    int oldTime = 0;          // 前フレームの時間（ミリ秒）

    float GetDeltaTime() const { return deltaTime; }
    float GetTotalTime() const { return totalTime; }

    Player player; // ← プレイヤーのメンバ変数
    Camera camera;
    Stage stage;
    ItemManager itemManager;
};
