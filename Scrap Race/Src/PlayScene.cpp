#include "PlayScene.h"
#include "SceneManager.h"
#include "Result.h"
#include "DxLib.h"

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

    DeleteGraph(SpeedMeter);
    DeleteGraph(MeterNeedle);
}

void PlayScene::Initialize()
{
    SpeedMeter = LoadGraph("Data/Image/SpeedMeter.png");
    MeterNeedle = LoadGraph("Data/Image/meterneedle.png");

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
            VGet(-100.0f, 0.0f, -15.0f),
            VGet(-100.0f, 0.0f, -250.0f),
            VGet(100.0f, 0.0f, -250.0f),
            VGet(100.0f, 0.0f, -15.0f)
        };
        enemies[i]->SetWaypoints(waypoints);
    }

    // Car配列を構築(PlaySceneで管理)
    BuildCarList();

    oldTime = GetNowCount();
    totalTime = 0.0f;
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

    DeleteGraph(SpeedMeter);
    DeleteGraph(MeterNeedle);
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

void PlayScene::Draw()
{
    //DxLib更新開始
    SetBackgroundColor(0, 140, 255);
    ClearDrawScreen();

    //Draw処理
    stage.Draw();
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
    DrawGraph(800,480,SpeedMeter,true);

    DrawRotaGraph(1030, 660, 1.0f, DX_PI_F / 180.0f*allCars[0]->moveSpeed, MeterNeedle, true);

    //ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    DrawPlayerDebugUI();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    ScreenFlip();
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

        SceneManager::ChangeScene(new Result(RESULT_GAMEOVER, data));
        return;
    }

    // ゴール判定
    if (stage.CheckGoal(player.pos))
    {
        ResultData data;
        data.raceTime = totalTime;
        data.finalSpeed = player.moveSpeed;
        data.finalHp = player.Hp;

        SceneManager::ChangeScene(new Result(RESULT_CLEAR, data));
        return;
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
