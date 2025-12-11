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
}

PlayScene::~PlayScene()
{   
}

void PlayScene::Initialize()
{
    stage.Initialize();
    player.Initialize();
    camera.Initialize();
    itemManager.Initialize();

    oldTime = GetNowCount();
    totalTime = 0.0f;
}

void PlayScene::Terminate() 
{
    stage.Terminate();
    camera.Terminate();
    itemManager.Terminate();
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
    SetBackgroundColor(140, 140, 140);
    ClearDrawScreen();

    //StageのUpdate
    stage.Draw();

    //Draw処理
    player.Draw();
    itemManager.Draw();

    //UI
    if (stage.CheckGoal(player.pos))
    {
        DrawString(0, 0, "GOAL!", GetColor(255, 255, 0));
    }

    if (player.Hp <= 0.0f) {
        printfDx("ゲームオーバー\n");
    }

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
    player.Update(deltaTime);
    camera.Update(player, deltaTime);
    itemManager.Update(player.pos, player.angle, deltaTime, player, stage.GetCheckColModel());
    stage.Update();
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

    ImGui::SliderFloat("Distance", &cameraDistance, 0.0f, 150.0f);
    ImGui::SliderFloat("Height", &cameraHeight, 0.0f, 100.0f);
    ImGui::SliderFloat("Target Offset Y", &targetOffsetY, -20.0f, 20.0f);
    ImGui::SliderFloat("cameraNear", &cameraNear, 0.0f, 1000.0f);
    ImGui::SliderFloat("cameraFar", &cameraFar, 0.0f, 1000.0f);
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
