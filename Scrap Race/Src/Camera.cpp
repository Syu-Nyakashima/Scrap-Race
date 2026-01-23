#include "Camera.h"
#include <math.h>

void Camera::Initialize()
{
    Eye = VGet(0.0f, 0.0f, 0.0f);
    Target = VGet(0.0f, 1.0f, 0.0f);

    smoothEye = Eye;
    smoothTarget = Target;

    // Near, Far クリップの距離を設定
    SetCameraNearFar(Near, Far);
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
    float playerAngle = player.angle;
    static float cameraAngle = playerAngle;

    // プレイヤーとカメラの角度差を計算(-180〜180度に正規化)
    float angleDiff = playerAngle - cameraAngle;
    while (angleDiff > 180.0f) angleDiff -= 360.0f;
    while (angleDiff < -180.0f) angleDiff += 360.0f;

    // ±20度のデッドゾーン
    const float deadZone = 20.0f;
    if (angleDiff > deadZone) {
        cameraAngle += angleDiff - deadZone;
    }
    else if (angleDiff < -deadZone) {
        cameraAngle += angleDiff + deadZone;
    }

    // カメラの角度を0〜360度に正規化
    while (cameraAngle >= 360.0f) cameraAngle -= 360.0f;
    while (cameraAngle < 0.0f) cameraAngle += 360.0f;

    float rad = cameraAngle * DX_PI_F / 180.0f;

    // 目標オフセット
    VECTOR offset = VGet(sinf(rad) * -Dist, Height, cosf(rad) * -Dist);
    VECTOR targetEye = VAdd(playerPos, offset);

    // 補間
    float lerpFactor = 1.0f - expf(-CAMERA_LERP_SPEED * delta);
    smoothEye = Lerp(smoothEye, targetEye, lerpFactor);

    // 距離を一定に保つ
    VECTOR toEye = VSub(smoothEye, playerPos);
    float currentDist = sqrtf(toEye.x * toEye.x + toEye.z * toEye.z);
    if (currentDist > 0.01f) {
        float scale = Dist / currentDist;
        toEye.x *= scale;
        toEye.z *= scale;
    }
    // Y方向の距離も補正
    float currentDistY = toEye.y;
    toEye.y = Height;

    Eye = VAdd(playerPos, toEye);
    Target = VAdd(playerPos, VGet(0.0f, TargetOffsetY, 0.0f));

    // 地面より下にならないように
    if (Eye.y < 5.0f) {
        Eye.y = 5.0f;
    }
    // 地面より下にならないように
    if (Eye.y < 5.0f) {
        Eye.y = 5.0f;
        smoothEye.y = 5.0f;  // smoothEyeも更新
    }

    SetCameraPositionAndTarget_UpVecY(Eye, Target);
    SetCameraNearFar(Near, Far);

    VECTOR forward = VGet(sinf(rad), 0.0f, cosf(rad));
    DrawLine3D(playerPos, VAdd(playerPos, VScale(forward, 200.0f)), GetColor(255, 255, 0)); // 黄線が前方向
}