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
    void Draw();            // 描画処理
    void UpdateGame();      // ゲームロジック更新
    void DrawPlayerDebugUI();
    void CheckGameEnd();    // ゲーム終了判定

private:
    // コンストラクタで初期化
    Player player;
    Stage stage;
    Camera camera;
    ItemManager itemManager;
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
    int oldTime = 0;
};