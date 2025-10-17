#include "PlayScene.h"
#include "DxLib.h"
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

//-------------------------------------------------------------
// コンストラクタ
//-------------------------------------------------------------
PlayScene::PlayScene()
{
    player.Player_Initialize();
    InitImGui();
}

//-------------------------------------------------------------
// デストラクタ
//-------------------------------------------------------------
PlayScene::~PlayScene()
{
    TerminateImGui();
    player.Player_Terminate();
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
    if (ImGui::TreeNode("Position")) {
        ImGui::SliderFloat("X", &player.pos.x, -100.0f, 100.0f);
        ImGui::SliderFloat("Y", &player.pos.y, -100.0f, 100.0f);
        ImGui::SliderFloat("Z", &player.pos.z, -100.0f, 100.0f);
        // 折りたたみ要素を終了
        ImGui::TreePop();
    }

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

    // --- 1. DxLib描画開始 ---
    ClearDrawScreen();
    player.Chara_Player_Create(); // モデル描画など

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