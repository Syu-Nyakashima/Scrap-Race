#include "PlayScene.h"
#include "SceneManager.h"
#include "Result.h"
#include "DxLib.h"
#include <algorithm>

#include <windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

PlayScene::PlayScene() : player(stage)
{
    //オブジェクトの構築、メンバ変数の初期化
     // 敵を3台生成(例)
    enemies.push_back(new EnemyCPU(stage, AIDifficulty::Easy, AIType::Attack));
    enemies.push_back(new EnemyCPU(stage, AIDifficulty::Normal, AIType::Balance));
    enemies.push_back(new EnemyCPU(stage, AIDifficulty::Hard, AIType::Defense));
    enemies.push_back(new EnemyCPU(stage, AIDifficulty::Normal, AIType::ScrapHunter));
}

PlayScene::~PlayScene()
{
    for (auto* enemy : enemies) {
        delete enemy;
    }
    enemies.clear();

    DeleteGraph(LowSpeedMeter);
    DeleteGraph(MeterNeedle);
    DeleteGraph(SpeedMeterBase);
    DeleteGraph(KmBase);
    DeleteGraph(LapAndRankBase);
    DeleteGraph(TimeUI);
    DeleteGraph(HighSpeedMeter);
    DeleteGraph(BrueHPMeter);
    DeleteGraph(YellowHPMeter);
    DeleteGraph(RedHPMeter);
    
    for (int i = 0; i < 10; i++) {
        DeleteGraph(RankUI[i]);
    }
    for (int i = 0; i < 4; i++) {
        DeleteGraph(lapUI[i]);
    }
}

void PlayScene::Initialize()
{
    //UI画像読み込み
    SpeedMeterBase = LoadGraph("Data/Image/_0009_Base.png");
    MeterNeedle = LoadGraph("Data/Image/_0008_hari.png");
    KmBase = LoadGraph("Data/Image/_0007_km-h.png");
    LapAndRankBase = LoadGraph("Data/Image/_0006_Lap_Th.png");
    TimeUI = LoadGraph("Data/Image/_0005_Time.png");

    LowSpeedMeter = LoadGraph("Data/Image/_0003_Speed_Blue_Bar2.png");
    HighSpeedMeter = LoadGraph("Data/Image/_0004_Speed_Red_Bar2.png");

    BrueHPMeter= LoadGraph("Data/Image/_0000_HP_Blue_Bar.png");
    YellowHPMeter= LoadGraph("Data/Image/_0001_HP_Yellow_Bar.png");
    RedHPMeter= LoadGraph("Data/Image/_0002_HP_Red_Bar.png");

    LoadDivGraph("Data/Image/RankNumber.png", 10, 10, 1, 130, 115, RankUI);
    LoadDivGraph("Data/Image/LapNumber.png", 10, 10, 1, 80, 70, lapUI);

    stage.Initialize();
    player.Initialize();
    camera.Initialize();
    itemManager.Initialize();

    // 敵の初期化
    for (int i = 0; i < enemies.size(); i++)
    {
        enemies[i]->Initialize();

        // 初期位置をずらす
        enemies[i]->pos = VGet(-5.0f * (i + 1), 10.0f, -10.0f);

        // ウェイポイント設定
        std::vector<VECTOR> waypoints = {
            VGet(0.0f, 0.0f, 0.0f),          // CP0: スタート/ゴール
            VGet(-100.0f, 0.0f, -15.0f),     // CP1
            VGet(-100.0f, 0.0f, -250.0f),    // CP2
            VGet(100.0f, 0.0f, -250.0f),     // CP3
            VGet(100.0f, 0.0f, -15.0f)       // CP4
        };
        enemies[i]->SetWaypoints(waypoints);
    }

    // Car配列を構築(PlaySceneで管理)
    BuildCarList();

    // 時間初期化
    oldTime = GetNowCount();
    totalTime = 0.0f;

    // 周回初期化
    lapStartTime = 0.0f;
    currentLapTime = 0.0f;
    bestLapTime = 999999.0f;
    lapTimes.clear();

    // 順位初期化
    rankings.resize(allCars.size());
    for (int i = 0; i < rankings.size(); i++) {
        rankings[i] = i;
    }
}

//全Car情報リスト生成
void PlayScene::BuildCarList()
{
    allCars.clear();

    // リストの最初にプレイヤーを追加
    allCars.push_back(&player);

    // 敵を追加
    for (auto* enemy : enemies) {
        allCars.push_back(enemy);
    }

    // 各車両にリストを設定
    for (auto* car : allCars)
    {
        car->SetCarList(&allCars);
    }
}

void PlayScene::Terminate() 
{
    stage.Terminate();
    camera.Terminate();
    itemManager.Terminate();

    for (auto* enemy : enemies) {
        enemy->Terminate();
    }

    allCars.clear();

    DeleteGraph(LowSpeedMeter);
    DeleteGraph(MeterNeedle);
    DeleteGraph(SpeedMeterBase);
    DeleteGraph(KmBase);
    DeleteGraph(LapAndRankBase);
    DeleteGraph(TimeUI);
    DeleteGraph(HighSpeedMeter);
    DeleteGraph(BrueHPMeter);
    DeleteGraph(YellowHPMeter);
    DeleteGraph(RedHPMeter);

    for (int i = 0; i < 10; i++) {
        DeleteGraph(RankUI[i]);
    }
    for (int i = 0; i < 4; i++) {
        DeleteGraph(lapUI[i]);
    }
    

}

//-------------------------------------------------------------
// 毎フレーム更新
//-------------------------------------------------------------
void PlayScene::Update()
{
    if (ProcessMessage() != 0) return;

    // デルタタイム計算
    int nowTime = GetNowCount();
    deltaTime = (nowTime - oldTime) / 1000.0f;
    oldTime = nowTime;
    totalTime += deltaTime;

    // ゲームロジック更新
    UpdateGame();

    // ゲーム終了判定
    CheckGameEnd();

    // 描画処理
    Draw();
}

void PlayScene::UpdateGame()
{
    //プレイヤー更新
    player.Update(deltaTime);
    //敵更新
    for (auto* enemy : enemies) 
    {
        if (enemy->IsAlive()) 
        {
            //更新
            enemy->Update(deltaTime);

            // 敵に最も近いスクラップを通知
            VECTOR nearestScrap;
            if (itemManager.FindNearestScrap(enemy->GetPosition(), 20.0f, nearestScrap))
            {
                enemy->SetNearestScrap(nearestScrap);
            }
            else
            {
                enemy->ClearNearestScrap();
            }
        }
    }

    camera.Update(player, deltaTime);
    stage.Update();
    itemManager.Update(deltaTime, stage.GetCheckColModel(), allCars);

    // 周回とラップタイム更新
    UpdateLaps();

    // 順位更新
    UpdateRankings();
}

void PlayScene::UpdateLaps()
{
    // 全車両のチェックポイントをチェック
    for (auto* car : allCars)
    {
        int currentCP = car->currentCheckpoint;

        // 次のチェックポイントに入ったか?
        if (stage.IsInsideCheckpoint(car->GetPosition(), currentCP))
        {
            int nextCP = (currentCP + 1) % stage.GetTotalCheckpoints();
            car->currentCheckpoint = nextCP;

            // CP0に戻った = ラップ完了
            if (nextCP == 0)
            {
                car->currentLap++;

                // プレイヤーのラップタイム記録
                if (car == &player)
                {
                    float lapTime = totalTime - lapStartTime;
                    lapTimes.push_back(lapTime);
                    currentLapTime = 0.0f;
                    lapStartTime = totalTime;

                    // ベストラップ更新
                    if (lapTime < bestLapTime)
                    {
                        bestLapTime = lapTime;
                    }
                }
            }
        }
    }

    // 現在のラップタイム更新
    currentLapTime = totalTime - lapStartTime;
}

void PlayScene::UpdateRankings()
{
    // ソート用の構造体
    struct RaceData
    {
        int index;
        int lap;
        int checkpoint;
        float progress;
    };

    std::vector<RaceData> raceData;

    for (int i = 0; i < allCars.size(); i++)
    {
        RaceData data;
        data.index = i;
        data.lap = allCars[i]->currentLap;
        data.checkpoint = allCars[i]->currentCheckpoint;
        data.progress = CalculateProgress(
            allCars[i]->GetPosition(),
            allCars[i]->currentCheckpoint
        );
        raceData.push_back(data);
    }

    // ソート: 周回数が多い順、同じなら通過CPが多い順
    std::sort(raceData.begin(), raceData.end(),
        [](const RaceData& a, const RaceData& b) 
        {
            // 1. 周回数が多い方が上位
            if (a.lap != b.lap) return a.lap > b.lap;

            // 2. 通過CPが多い方が上位
            if (a.checkpoint != b.checkpoint) return a.checkpoint > b.checkpoint;

            // 3. 同じLap, 同じCPなら進行度が高い方が上位
            return a.progress > b.progress;
        });

    // 順位配列を更新
    for (int i = 0; i < raceData.size(); i++)
    {
        rankings[i] = raceData[i].index;
    }
}

float PlayScene::CalculateProgress(VECTOR carPos, int currentCheckpoint)
{
    int totalCheckpoints = stage.GetTotalCheckpoints();
    if (totalCheckpoints == 0) return 0.0f;

    // 現在のCPから次のCPへのベクトル
    VECTOR currentCP = stage.GetCheckpointPos(currentCheckpoint);
    int nextCheckpointIndex = (currentCheckpoint + 1) % totalCheckpoints;
    VECTOR nextCP = stage.GetCheckpointPos(nextCheckpointIndex);

    // CPからCPへのベクトル
    VECTOR cpToNext = VSub(nextCP, currentCP);
    float totalDist = VSize(cpToNext);

    if (totalDist < 0.01f) return 0.0f;  // CPが同じ位置

    // 車の位置から次のCPまでの距離
    VECTOR carToNext = VSub(nextCP, carPos);
    float distToNext = VSize(carToNext);

    // 進行度 = (総距離 - 残り距離) / 総距離
    float progress = (totalDist - distToNext) / totalDist;

    // 0.0~1.0にクランプ
    if (progress < 0.0f) progress = 0.0f;
    if (progress > 1.0f) progress = 1.0f;

    return progress;
}

void PlayScene::CheckGameEnd()
{
    // ゲームオーバー判定
    if (player.Hp <= 0.0f)
    {
        ResultData data;
        data.raceTime = totalTime;
        data.finalSpeed = player.moveSpeed;
        data.finalHp = player.Hp;

        isGameEnd = true;
        SceneManager::ChangeScene(new Result(RESULT_GAMEOVER, data));
        return;
    }

    // ゴール判定
    if (player.currentLap > TOTAL_LAPS)
    {
        ResultData data;
        data.raceTime = totalTime;
        data.finalSpeed = player.moveSpeed;
        data.finalHp = player.Hp;

		isGameEnd = true;
        SceneManager::ChangeScene(new Result(RESULT_CLEAR, data));
        return;
    }
}

void PlayScene::Draw()
{
    //DxLib更新開始
    SetBackgroundColor(0, 140, 255);
    ClearDrawScreen();

    //Draw処理
    stage.Draw();
    stage.DrawCheckpoints();
    player.Draw();

    // 敵描画
    for (auto* enemy : enemies)
    {
        if (enemy->IsAlive())
        {
            enemy->Draw();
        }
    }

    itemManager.Draw();

    //UI
    DrawRaceUI();
    
    //ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    //DrawPlayerDebugUI();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    ScreenFlip();
}

void PlayScene::DrawRaceUI()
{
    // ベース画像を描画
    DrawGraph(1300, 600, SpeedMeterBase, true);
    DrawGraph(1390, 750, KmBase, true);

    // スピード比率を計算
    float speedRatio = allCars[0]->moveSpeed / allCars[0]->MAX_SPD_MAX;

    // 円弧状のスピードメーター画像を描画
    // LowSpeedMeter or HighSpeedMeter を速度に応じて描画
    if (speedRatio < 0.5f) {
        // 低速時は青いメーター
        DrawArcImageMeter(
            1422, 722,            // 中心座標
            LowSpeedMeter,       // 画像ハンドル
            speedRatio * 2.0f,   // 0.5以下なので2倍して0~1にする
            85.0f,               // 内側半径
            95.0f,               // 外側半径
            -170.0f,             // 開始角度（左下）
            185.0f               // 角度範囲（左下から左上まで135度）
        );
    }
    else {
        // 低速メーターは満タン
        DrawArcImageMeter(1422, 722,   LowSpeedMeter, 1.0f,
            85.0f, 95.0f, -170.0f, 190.0f);

        // 高速時は赤いメーター
        float highSpeedRatio = (speedRatio - 0.5f) * 2.0f; // 0.5~1を0~1に変換
        DrawArcImageMeter(
            1422, 722,
            HighSpeedMeter,
            highSpeedRatio,
            85.0f,               // 内側半径
            95.0f,
            15.0f,                // 開始角度（上）
            60.0f               // 角度範囲（上から右下まで135度）
        );
    }

    // 針の描画
    float needleAngle = -87.0f + (270.0f * speedRatio);
    DrawRotaGraph2(1422, 722, 2, 52, 1.0f,
        needleAngle * DX_PI_F / 180.0f,
        MeterNeedle, true, false);

    // HPメーターも同様に描画
    float hpRatio = allCars[0]->Hp / 100.0f;
    int hpMeterHandle;

    if (hpRatio > 0.6f) {
        hpMeterHandle = BrueHPMeter;
    }
    else if (hpRatio > 0.3f) {
        hpMeterHandle = YellowHPMeter;
    }
    else {
        hpMeterHandle = RedHPMeter;
    }

    DrawArcImageMeter(
        1422, 722,           // 中心座標
        hpMeterHandle,      // HP画像
        hpRatio,            // 表示割合
        105.0f,              // 内側半径
        125.0f,             // 外側半径
        -170.0f,            // 開始角度
        240.0f              // 角度範囲（全周）
    );

    // その他のUI
    DrawGraph(50, 600, LapAndRankBase, true);
    //DrawGraph(10, 10, TimeUI, true);

    // 順位UI
    int playerRank = 1;
    for (int i = 0; i < rankings.size(); i++) {
        if (rankings[i] == 0) {
            playerRank = i + 1;
            break;
        }
    }
    DrawGraph(330, 675, RankUI[playerRank], true);

    // ラップUI
    DrawGraph(70, 670, lapUI[player.currentLap], true);
    DrawGraph(180, 710, lapUI[TOTAL_LAPS], true);

    // ラップ、タイム、順位などのテキスト表示
    DrawFormatString(10, 10, GetColor(255, 255, 255),
        "LAP TIME: %.2f", currentLapTime);

    if (bestLapTime < 999999.0f) {
        DrawFormatString(10, 30, GetColor(255, 255, 0),
            "BEST LAP: %.2f", bestLapTime);
    }
}

void PlayScene::DrawArcImageMeter(int centerX, int centerY, int graphHandle, float ratio, float innerRadius, float outerRadius, float startAngleDeg, float totalAngleDeg)
{
    // ratioをクランプ
    if (ratio < 0.0f) ratio = 0.0f;
    if (ratio > 1.0f) ratio = 1.0f;

    // 画像サイズを取得
    int imgW, imgH;
    GetGraphSize(graphHandle, &imgW, &imgH);

    // 分割数（細かいほど滑らか、重いほど処理が増える）
    int segments = 50;

    // 実際に描画する角度
    float drawAngleDeg = totalAngleDeg * ratio;

    // ラジアンに変換
    float startAngle = startAngleDeg * DX_PI_F / 180.0f;
    float angleStep = (drawAngleDeg * DX_PI_F / 180.0f) / segments;

    // 各セグメントを四角形として描画
    for (int i = 0; i < segments; i++)
    {
        float angle1 = startAngle + angleStep * i;
        float angle2 = startAngle + angleStep * (i + 1);

        // UV座標の計算（画像の左から右へ進む）
        float u1 = (float)i / segments;
        float u2 = (float)(i + 1) / segments;

        // 4点の座標を計算
        // 左下（内側、angle1）
        int x1 = centerX + (int)(cos(angle1) * innerRadius);
        int y1 = centerY + (int)(sin(angle1) * innerRadius);

        // 左上（外側、angle1）
        int x2 = centerX + (int)(cos(angle1) * outerRadius);
        int y2 = centerY + (int)(sin(angle1) * outerRadius);

        // 右上（外側、angle2）
        int x3 = centerX + (int)(cos(angle2) * outerRadius);
        int y3 = centerY + (int)(sin(angle2) * outerRadius);

        // 右下（内側、angle2）
        int x4 = centerX + (int)(cos(angle2) * innerRadius);
        int y4 = centerY + (int)(sin(angle2) * innerRadius);

        DrawPixel(centerX,centerY, GetColor(0, 255, 0));

        DrawRectModiGraph(
            x1, y1, x2, y2, x3, y3, x4, y4,  // 描画先の4点
            (int)(u1 * imgW), 0,              // 画像の切り取り開始位置
            (int)((u2 - u1) * imgW), imgH,    // 切り取りサイズ
            graphHandle, TRUE
        );
    }
}

//-------------------------------------------------------------
// PlaySceneデバッグUI
//-------------------------------------------------------------
void PlayScene::DrawPlayerDebugUI()
{
    ImGui::SetNextWindowSize(ImVec2(400, 500), ImGuiCond_Once);

    ImGui::Begin("Player Debug");

    //プレイヤー座標操作
    ImGui::Separator();
    ImGui::Text("Player Position");
    ImGui::SliderFloat("X", &player.pos.x, -100.0f, 100.0f);
    ImGui::SliderFloat("Y", &player.pos.y, -100.0f, 100.0f);
    ImGui::SliderFloat("Z", &player.pos.z, -100.0f, 100.0f);

    //カメラ座標操作
    ImGui::Separator();
    ImGui::Text("Camera Position");
    static float cameraDistance = 15.0f;
    static float cameraHeight = 0.0f;
    static float targetOffsetY = 0.0f;
    static float cameraNear = 0.1f;
    static float cameraFar = 500.0f;

    ImGui::SliderFloat("Distance", &cameraDistance, 0.0f, 1500.0f);
    ImGui::SliderFloat("Height", &cameraHeight, 0.0f, 1000.0f);
    ImGui::SliderFloat("Target Offset Y", &targetOffsetY, -20.0f, 1020.0f);
    ImGui::SliderFloat("cameraNear", &cameraNear, 0.0f, 10000.0f);
    ImGui::SliderFloat("cameraFar", &cameraFar, 0.0f, 10000.0f);
    // 値をCameraクラスに渡す（TPS視点用に）
    camera.SetDebugCameraParams(cameraDistance, cameraHeight, targetOffsetY, cameraNear, cameraFar);

    //コリジョン判定
    ImGui::Separator();
    ImGui::Text("Collision Settings");
    ImGui::SliderFloat("Capsule Radius", &player.capsuleRadius, 0.5f, 5.0f);
    ImGui::SliderFloat("Capsule Height", &player.capsuleHeight, 1.0f, 10.0f);

    // デルタタイム
    ImGui::Separator();
    ImGui::Text("DeltaTime");
    ImGui::InputFloat("DeltaTime", &totalTime, 0.0f, 1000.0f);

    // ステータス
    ImGui::Separator();
    ImGui::Text("Status");
    ImGui::InputFloat("Move Speed", &player.moveSpeed, 0.0f, 100.0f);
    ImGui::InputFloat("Move Speed", &player.SpdMax, 0.0f, 100.0f);
    ImGui::InputFloat("HP", &player.Hp, 0.0f, 100.0f);

    // 位置リセット
    if (ImGui::Button("Reset Position"))
    {
        player.pos = VGet(0.0f, 0.0f, 0.0f);
        player.angle = 0;
        cameraDistance = 15.0f;
        cameraHeight = 0.0f;
        targetOffsetY = 0.0f;
        cameraNear = 0.1f;
        cameraFar = 500.0f;
    }

    ImGui::End();
}

