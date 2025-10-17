#pragma once
#include "Player.h"

class PlayScene
{
public:
    PlayScene();
    ~PlayScene();

    void Update(); // ループ内の処理を書く関数
private:
    void InitImGui();
    void TerminateImGui();
    void DrawPlayerDebugUI();
    Player player; // ← プレイヤーのメンバ変数
};
