#pragma once
#include "Player.h"
#include "Camera.h"
#include "Stage.h"
#include "ItemManager.h"

class PlayScene
{
public:
    PlayScene();   // コンストラクタ
    ~PlayScene();  // デストラクタ

    void Initialize();
    void Update();

private:
   
    void DrawPlayerDebugUI();

    float deltaTime = 0.0f;
    float totalTime = 0.0f;
    int oldTime = 0;
    float GetDeltaTime() const { return deltaTime; }
    float GetTotalTime() const { return totalTime; }

    // コンストラクタで初期化
    Player player;
    Stage stage;
    Camera camera;
    ItemManager itemManager;
    
};