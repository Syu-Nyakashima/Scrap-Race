#include "Camera.h"
#include <math.h>

void Camera::Initialize()
{
    Eye = VGet(0.0f, 0.0f, 0.0f);
    Target = VGet(0.0f, 1.0f, 0.0f);
    isDebugOverView = false;

    smoothEye = Eye;
    smoothTarget = Target;

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
    VECTOR playerPos = player.pos;

     // プレイヤーの角度（ラジアン）
    float rad = player.angle * DX_PI_F / 180.0f;

    // TPS視点（後ろから追従）
    VECTOR offset = VGet(sinf(rad) * -debugDist, debugHeight, cosf(rad) * -debugDist);

    // 目標位置を計算(即座には適用しない)
    VECTOR targetEye = VAdd(playerPos, offset);
    VECTOR targetTarget = VAdd(playerPos, VGet(0.0f, debugTargetOffsetY, 0.0f));

    // 指数関数的補間(フレームレート独立)
    float lerpFactor = 1.0f - expf(-CAMERA_LERP_SPEED * delta);

    // 滑らかに補間
    smoothEye = Lerp(smoothEye, targetEye, lerpFactor);
    smoothTarget = Lerp(smoothTarget, targetTarget, lerpFactor);

    // 補間後の値を使用
    Eye = smoothEye;
    Target = smoothTarget;

    // 地面より下にならないように
    if (Eye.y < 5.0f) {
        Eye.y = 5.0f;
        smoothEye.y = 5.0f;  // smoothEyeも更新
    }

    SetCameraPositionAndTarget_UpVecY(Eye, Target);
    SetCameraNearFar(debugNear, debugFar);

    VECTOR forward = VGet(sinf(rad), 0.0f, cosf(rad));
    DrawLine3D(playerPos, VAdd(playerPos, VScale(forward, 200.0f)), GetColor(255, 255, 0)); // 黄線が前方向
}