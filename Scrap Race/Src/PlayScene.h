#pragma once
#include "Player.h"

class PlayScene
{
public:
    PlayScene();
    ~PlayScene();

    void Update(); // ���[�v���̏����������֐�
private:
    void InitImGui();
    void TerminateImGui();
    void DrawPlayerDebugUI();
    Player player; // �� �v���C���[�̃����o�ϐ�
};
