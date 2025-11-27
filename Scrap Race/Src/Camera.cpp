#include "Camera.h"
#include <math.h>

void Camera::Initialize()
{
    Eye = VGet(0.0f, 0.0f, 0.0f);
    Target = VGet(0.0f, 1.0f, 0.0f);
    isDebugOverView = false;

    // Near, Far クリップの距離を設定
    SetCameraNearFar(debugNear, debugFar);
}

void Camera::Terminate() 
{
}

static VECTOR Lerp(const VECTOR& a, const VECTOR& b, float t)
{
    return VAdd(a, VScale(VSub(b, a), t));
}


void Camera::Update(const Player& player, float delta)
{
    //DrawSphere3D(Eye, 10.0f, 16, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);

    VECTOR playerPos = player.pos;

     // プレイヤーの角度（ラジアン）
    float rad = player.angle * DX_PI_F / 180.0f;

    // TPS視点（後ろから追従）
    VECTOR offset = VGet(sinf(rad) * -debugDist, debugHeight, cosf(rad) * -debugDist);

    Eye = VAdd(playerPos, offset);       
    Target = VAdd(playerPos, VGet(0.0f, debugTargetOffsetY, 0.0f));
   
    // 滑らか追従（deltaを使ってフレームレート補正）
    //Eye = Lerp(Eye, targetEye, 0.2f * delta * 60.0f);
    //Target = Lerp(Target, targetTarget, 0.2f * delta * 60.0f);

    if (Eye.y < 5.0f) Eye.y = 5.0f;

    SetCameraPositionAndTarget_UpVecY(Eye, Target);
    SetCameraNearFar(debugNear, debugFar);

    VECTOR forward = VGet(sinf(rad), 0.0f, cosf(rad));
    DrawLine3D(playerPos, VAdd(playerPos, VScale(forward, 200.0f)), GetColor(255, 255, 0)); // 黄線が前方向
}

void Camera::ToggleDebugOverView()
{
    isDebugOverView = !isDebugOverView;
}
