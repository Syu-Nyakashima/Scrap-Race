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
    void DrawRaceUI();
    void BuildCarList();    // Car配列を構築
    void UpdateGame();      // ゲームロジック更新
    void CheckGameEnd();    // ゲーム終了判定
    void DrawPlayerDebugUI();
    void DrawArcImageMeter(int centerX, int centerY, int graphHandle,
        float ratio, float innerRadius, float outerRadius,
        float startAngleDeg, float totalAngleDeg);

    void UpdateLaps();      //周回管理
    void UpdateRankings();  //順位管理
    float CalculateProgress(VECTOR carPos, int currentCheckpoint);
    
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

    //周回管理
    float lapStartTime;           // ラップ開始時刻
    float currentLapTime;         // 現在のラップタイム
    float bestLapTime;            // ベストラップ

    // リスト
    std::vector<EnemyCPU*> enemies;     //敵リスト
    std::vector<CarBase*> allCars;      //全Car管理
    std::vector<float> lapTimes;  // 各ラップのタイム
    std::vector<int> rankings;    // 順位配列 rankings[0]=1位の車のindex
    
    //UI画像
    int SpeedMeterBase;
    int MeterNeedle;
    int KmBase;
    int LapAndRankBase;
    int TimeUI;

    int LowSpeedMeter;
    int HighSpeedMeter;

    int BrueHPMeter;
    int YellowHPMeter;
    int RedHPMeter;

    int RankUI[10];
    int lapUI[10];
    
	//終了フラグ
    bool isGameEnd = false;

    //定数
    static constexpr int TOTAL_LAPS = 3;  // 総周回数
};