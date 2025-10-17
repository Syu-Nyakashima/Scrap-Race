#include "DxLib.h"
#include "PlayScene.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <windows.h>

//---------------------------------------------
// �O���[�o��
//---------------------------------------------
HWND g_hWnd = nullptr;
WNDPROC g_OriginalWndProc = nullptr;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//---------------------------------------------
// ImGui�ɓ��͂�n��WndProc
//---------------------------------------------
LRESULT CALLBACK CustomWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // ImGui��Win32���b�Z�[�W��n��
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return TRUE; // ImGui�����������Ȃ炱���ŏI��

    // DxLib�{���̏������Ăяo��
    return CallWindowProc(g_OriginalWndProc, hWnd, msg, wParam, lParam);
}

//---------------------------------------------
// �G���g���[�|�C���g
//---------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // --- DxLib�����ݒ� ---
    ChangeWindowMode(TRUE);                        // �E�B���h�E���[�h
    SetGraphMode(1280, 720, 32);                   // �`��𑜓x�i��, ����, �J���[�r�b�g���j
    SetWindowSizeExtendRate(1.0);                  // �E�B���h�E�̊g�嗦�i1.0 = ���{�j
    SetMainWindowText("�X�N���b�v���[�X");         // �E�B���h�E�^�C�g��
    SetUseDirect3DVersion(DX_DIRECT3D_11);         // �g�p�o�[�W�����w��

    if (DxLib_Init() == -1)
        return -1;

    // --- WndProc��ImGui�Ή��ɍ����ւ� ---
    g_hWnd = GetMainWindowHandle();
    g_OriginalWndProc = (WNDPROC)GetWindowLongPtr(g_hWnd, GWLP_WNDPROC);
    SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)CustomWndProc);

    // --- �}�E�X�J�[�\����\�� ---
    SetUseDirectInputFlag(FALSE);  // ImGui�Ƃ̍��W�Y���h�~
    SetMouseDispFlag(TRUE);        // ��Ƀ}�E�X�\��

    // --- �V�[���J�n ---
    PlayScene* scene = new PlayScene();

    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0)
    {
        scene->Update();
    }

    delete scene;
    scene = nullptr;

    // --- �I������ ---
    DxLib_End();
    return 0;
}
