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
    InitImGui();

}

void PlayScene::Terminate()
{
    player.Player_Terminate();
    camera.Camera_Terminate();
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
// PlayerデバッグUI
//-------------------------------------------------------------
void PlayScene::DrawPlayerDebugUI()
{
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);

    ImGui::Begin("Player Debug");

    // 座標操作
    ImGui::Separator();
    ImGui::Text("Player Position");
    ImGui::SliderFloat("X", &player.pos.x, -100.0f, 100.0f);
    ImGui::SliderFloat("Y", &player.pos.y, -100.0f, 100.0f);
    ImGui::SliderFloat("Z", &player.pos.z, -100.0f, 100.0f);
        
    ImGui::Separator();
    ImGui::Text("Camera Position");
    static float cameraDistance = 50.0f;
    static float cameraHeight = 20.0f;
    static float targetOffsetY = 5.0f;

    ImGui::SliderFloat("Distance", &cameraDistance, 0.0f, 150.0f);
    ImGui::SliderFloat("Height", &cameraHeight, 0.0f, 100.0f);
    ImGui::SliderFloat("Target Offset Y", &targetOffsetY, -20.0f, 20.0f);
    // 値をCameraクラスに渡す（TPS視点用に）
    camera.SetDebugCameraParams(cameraDistance, cameraHeight, targetOffsetY);

    // 角度
    ImGui::Separator();
    ImGui::Text("Rotation");
    ImGui::SliderFloat("Angle", &player.angle, 0.0f, 360.0f);

    // 移動速度
    ImGui::Separator();
    ImGui::Text("Speed");
    ImGui::SliderFloat("Move Speed", &player.moveSpeed, 0.0f, 100.0f);

    // 位置リセット
    if (ImGui::Button("Reset Position"))
    {
        player.pos = VGet(0.0f, 0.0f, 0.0f);
    }

    ImGui::End();
}

//-------------------------------------------------------------
// 毎フレーム更新
//-------------------------------------------------------------
void PlayScene::Update()
{
    if (ProcessMessage() != 0) return;

    static int oldTime = GetNowCount();
    int nowTime = GetNowCount();
    float delta = (nowTime - oldTime) / 1000.0f; // 秒
    oldTime = nowTime;

    // --- 1. DxLib描画開始 ---
    SetBackgroundColor(140, 140, 140); // 灰色に   変更
    ClearDrawScreen();
    player.Player_Update(delta); // Player更新

    // Tabキーで視点切り替え
    static bool prevTab = false;
    bool nowTab = (CheckHitKey(KEY_INPUT_TAB) != 0);
    if (nowTab && !prevTab) camera.ToggleDebugOverView();
    prevTab = nowTab;

    camera.Update(player, delta);      // カメラの更新
    
    //Playerモデル更新
    player.Player_Draw();

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