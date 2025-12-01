#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Camera.h"
#include "Stage.h"
#include "ItemManager.h"

class PlayScene : public BaseScene
{
public:
    PlayScene();   // コンストラクタ
    ~PlayScene();  // デストラクタ

    void Initialize() override;
    void Terminate() override;
    void Update() override;

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