#include "PlayScene.h"
#include "DxLib.h"
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"



void PlayScene::Initialize()
{
    player.Player_Initialize();
    camera.Camera_Initialize();
    stage.Stage_Initialize();
    itemManager.ItemManager_Initialize();

    InitImGui();

    oldTime = GetNowCount();
}

void PlayScene::Terminate()
{
    player.Player_Terminate();
    camera.Camera_Terminate();
    stage.Stage_Terminate();
    itemManager.ItemManager_Terminate();
    
    TerminateImGui();
}

//-------------------------------------------------------------
// ImGui 初期化
//-------------------------------------------------------------
void PlayScene::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    HWND hwnd = GetMainWindowHandle();

    // DxLib内部のDirectX11デバイスを取得
    ID3D11Device* device = (ID3D11Device*)GetUseDirect3D11Device();
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)GetUseDirect3D11DeviceContext();

    if (device && context)
    {
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(device, context);
    }
}

//-------------------------------------------------------------
// ImGui 終了処理
//-------------------------------------------------------------
void PlayScene::TerminateImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
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
    static float cameraNear=0.1f;
    static float cameraFar=500.0f;

    ImGui::SliderFloat("Distance", &cameraDistance, 0.0f, 150.0f);
    ImGui::SliderFloat("Height", &cameraHeight, 0.0f, 100.0f);
    ImGui::SliderFloat("Target Offset Y", &targetOffsetY, -20.0f, 20.0f);
    ImGui::SliderFloat("cameraNear", &cameraNear, 0.0f, 1000.0f);
    ImGui::SliderFloat("cameraFar", &cameraFar, 0.0f, 1000.0f);
    // 値をCameraクラスに渡す（TPS視点用に）
    camera.SetDebugCameraParams(cameraDistance, cameraHeight, targetOffsetY,cameraNear,cameraFar);


    // 角度
    ImGui::Separator();
    ImGui::Text("DeltaTime");
    ImGui::InputFloat("DeltaTime",&totalTime , 0.0f, 1000.0f);

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
        cameraDistance = 0.0f;
        cameraHeight = 0.0f;
        targetOffsetY = 0.0f;
    }

    ImGui::End();
}

//-------------------------------------------------------------
// 毎フレーム更新
//-------------------------------------------------------------
void PlayScene::Update()
{
    if (ProcessMessage() != 0) return;

    int nowTime = GetNowCount();
    deltaTime = (nowTime - oldTime) / 1000.0f; // 秒
    oldTime = nowTime;
    totalTime += deltaTime;

    // --- 1. DxLib描画開始 ---
    SetBackgroundColor(140, 140, 140);
    ClearDrawScreen();

    player.Player_Update(deltaTime);
    camera.Camera_Update(player, deltaTime);    // カメラの更新
    itemManager.ItemManager_Update(player.pos, deltaTime, player);
    
    //Stage更新
    stage.Stage_Update();
    stage.Stage_Draw();

    //Playerモデル更新
    player.Player_Draw();
    itemManager.ItemManager_Draw();

    //UI
    if (stage.CheckGoal(player.pos))
    {
        DrawString(0, 0, "GOAL!", GetColor(255, 255, 0));
    }

    if (player.Hp <= 0.0f) {
        printfDx("ゲームオーバー\n");
    }
    // --- 2. ImGuiフレーム開始 ---
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // --- 3. ImGui UI ---
    DrawPlayerDebugUI();

    // --- 4. ImGui描画 ---
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // --- 5. 画面反映 ---
    ScreenFlip();
}