#include "DxLib.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <windows.h>

//---------------------------------------------
// グローバル
//---------------------------------------------
HWND g_hWnd = nullptr;
WNDPROC g_OriginalWndProc = nullptr;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//---------------------------------------------
// ImGuiに入力を渡すWndProc
//---------------------------------------------
LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // ImGuiにWin32メッセージを渡す
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return TRUE; // ImGuiが処理したならここで終了

    // DxLib本来の処理を呼び出す
    return CallWindowProc(g_OriginalWndProc, hWnd, msg, wParam, lParam);
}


//-------------------------------------------------------------
// ImGui 初期化
//-------------------------------------------------------------
void InitImGui()
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
void TerminateImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

//---------------------------------------------
// エントリーポイント
//---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // --- DxLib初期設定 ---
    ChangeWindowMode(TRUE);                        // ウィンドウモード
    SetGraphMode(1280,720, 32);                   // 描画解像度（幅, 高さ, カラービット数）
    SetWindowSizeChangeEnableFlag(TRUE);           // ウインドウのサイズを自由に変更
    SetWindowSizeExtendRate(1.0);                  // ウィンドウの拡大率（1.0 = 等倍）
    SetMainWindowText("スクラップレース");         // ウィンドウタイトル
    SetUseDirect3DVersion(DX_DIRECT3D_11);         // 使用バージョン指定
    
    if (DxLib_Init() == -1)
        return -1;

    // --- WndProcをImGui対応に差し替え ---
    g_hWnd = GetMainWindowHandle();
    g_OriginalWndProc = (WNDPROC)GetWindowLongPtr(g_hWnd, GWLP_WNDPROC);
    SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);

    // --- マウスカーソルを表示 ---
    SetUseDirectInputFlag(FALSE);  // ImGuiとの座標ズレ防止
    SetMouseDispFlag(TRUE);        // 常にマウス表示

    // --- シーン開始 ---
    SceneManager::Initialize();
    SceneManager::ChangeScene(new TitleScene());
    void InitImGui();
    void TerminateImGui();
    InitImGui();

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        SceneManager::Update();
    }

    TerminateImGui();

    // --- 終了処理 ---
    DxLib_End();
    return 0;
}
