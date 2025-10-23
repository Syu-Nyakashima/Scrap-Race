#pragma once
#include "Player.h"
#include "Camera.h"
#include "Stage.h"

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

    Player player; // ← プレイヤーのメンバ変数
    Camera camera;
    Stage stage;
};
