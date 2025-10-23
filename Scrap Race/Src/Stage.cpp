#include "Stage.h"

void Stage::Stage_Initialize()
{
}

void Stage::Stage_Terminate()
{
}

void Stage::Stage_Update()
{
    // ===== ínñ Çï`âÊ =====
    VECTOR p1 = VGet(-50.0f, 0.0f, -200.0f);
    VECTOR p2 = VGet(50.0f, 0.0f, -200.0f);
    VECTOR p3 = VGet(-50.0f, 0.0f, 200.0f);
    VECTOR p4 = VGet(50.0f, 0.0f, 200.0f);
    int groundColor = GetColor(100, 200, 100);
    DrawTriangle3D(p1, p2, p3, groundColor, TRUE);
    DrawTriangle3D(p3, p2, p4, groundColor, TRUE);

    // ===== ç∂ÇÃï« =====
    VECTOR lw1 = VGet(-50.0f, 0.0f, -200.0f);
    VECTOR lw2 = VGet(-50.0f, 3.0f, -200.0f);
    VECTOR lw3 = VGet(-50.0f, 0.0f, 200.0f);
    VECTOR lw4 = VGet(-50.0f, 3.0f, 200.0f);
    int wallColor = GetColor(150, 150, 150);
    DrawTriangle3D(lw1, lw2, lw3, wallColor, TRUE);
    DrawTriangle3D(lw3, lw2, lw4, wallColor, TRUE);

    // ===== âEÇÃï« =====
    VECTOR rw1 = VGet(50.0f, 0.0f, -200.0f);
    VECTOR rw2 = VGet(50.0f, 3.0f, -200.0f);
    VECTOR rw3 = VGet(50.0f, 0.0f, 200.0f);
    VECTOR rw4 = VGet(50.0f, 3.0f, 200.0f);
    DrawTriangle3D(rw1, rw3, rw2, wallColor, TRUE);
    DrawTriangle3D(rw3, rw4, rw2, wallColor, TRUE);

    for (float z = -200; z < 200; z+=10.0f) {
        DrawCone3D(VGet(51.0f, 10.0f, z), VGet(51.0f, 0.0f, z), 3.0f, 16, GetColor(0, 0, 255), GetColor(255, 255, 255), TRUE);
        DrawCone3D(VGet(-51.0f, 10.0f, z), VGet(-51.0f, 0.0f, z), 3.0f, 16, GetColor(0, 0, 255), GetColor(255, 255, 255), TRUE);
    }
}
