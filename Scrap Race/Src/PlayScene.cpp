#include "PlayScene.h"
#include "DxLib.h"
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

//-------------------------------------------------------------
// �R���X�g���N�^
//-------------------------------------------------------------
PlayScene::PlayScene()
{
    player.Player_Initialize();
    InitImGui();
}

//-------------------------------------------------------------
// �f�X�g���N�^
//-------------------------------------------------------------
PlayScene::~PlayScene()
{
    TerminateImGui();
    player.Player_Terminate();
}

//-------------------------------------------------------------
// ImGui ������
//-------------------------------------------------------------
void PlayScene::InitImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    HWND hwnd = GetMainWindowHandle();

    // DxLib������DirectX11�f�o�C�X���擾
    ID3D11Device* device = (ID3D11Device*)GetUseDirect3D11Device();
    ID3D11DeviceContext* context = (ID3D11DeviceContext*)GetUseDirect3D11DeviceContext();

    if (device && context)
    {
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(device, context);
    }
}

//-------------------------------------------------------------
// ImGui �I������
//-------------------------------------------------------------
void PlayScene::TerminateImGui()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

//-------------------------------------------------------------
// Player�f�o�b�OUI
//-------------------------------------------------------------
void PlayScene::DrawPlayerDebugUI()
{
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);

    ImGui::Begin("Player Debug");

    // ���W����
    if (ImGui::TreeNode("Position")) {
        ImGui::SliderFloat("X", &player.pos.x, -100.0f, 100.0f);
        ImGui::SliderFloat("Y", &player.pos.y, -100.0f, 100.0f);
        ImGui::SliderFloat("Z", &player.pos.z, -100.0f, 100.0f);
        // �܂肽���ݗv�f���I��
        ImGui::TreePop();
    }

    // �p�x
    ImGui::Separator();
    ImGui::Text("Rotation");
    ImGui::SliderFloat("Angle", &player.angle, 0.0f, 360.0f);

    // �ړ����x
    ImGui::Separator();
    ImGui::Text("Speed");
    ImGui::SliderFloat("Move Speed", &player.moveSpeed, 0.0f, 100.0f);

    // �ʒu���Z�b�g
    if (ImGui::Button("Reset Position"))
    {
        player.pos = VGet(0.0f, 0.0f, 0.0f);
    }

    ImGui::End();
}

//-------------------------------------------------------------
// ���t���[���X�V
//-------------------------------------------------------------
void PlayScene::Update()
{
    if (ProcessMessage() != 0) return;

    // --- 1. DxLib�`��J�n ---
    ClearDrawScreen();
    player.Chara_Player_Create(); // ���f���`��Ȃ�

    // --- 2. ImGui�t���[���J�n ---
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // --- 3. ImGui UI ---
    DrawPlayerDebugUI();

    // --- 4. ImGui�`�� ---
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // --- 5. ��ʔ��f ---
    ScreenFlip();
}