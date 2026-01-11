#pragma once
#include "BaseScene.h"
#include "Player.h"
#include "Camera.h"
#include "Stage.h"
#include "ItemManager.h"
#include "EnemyCPU.h"
#include <vector>

class PlayScene : public BaseScene
{
public:
    PlayScene();   // コンストラクタ
    ~PlayScene() override;  // デストラクタ

    void Initialize() override;
    void Terminate() override;
    void Update() override;

private:
    void Draw();            // 描画処理
    void BuildCarList();    // Car配列を構築
    void UpdateGame();      // ゲームロジック更新
    void CheckGameEnd();    // ゲーム終了判定
    void DrawPlayerDebugUI();
private:
    // コンストラクタで初期化
    // ゲームオブジェクト
    Player player;
    Stage stage;
    Camera camera;
    ItemManager itemManager;

    // 時間管理
    float deltaTime = 0.0f;
    float totalTime = 0.0f;
    int oldTime = 0;

    // リスト
    std::vector<EnemyCPU*> enemies;     //敵リスト
    std::vector<CarBase*> allCars;      //全Car管理

    //UI画像
    int SpeedMeter;
    int MeterNeedle;
    int RankNum;
    int RapNum;

};