#pragma once
#include "DxLib.h"
#include "Player.h"

class Camera {
public:
	void Camera_Initialize();
	void Camera_Terminate();
	void Camera_Update(const Player& player, float delta);

	// デバッグ切り替え
	void ToggleDebugOverView();

	void SetDebugCameraParams(float dist, float height, float offsetY) {
		debugDist = dist;
		debugHeight = height;
		debugTargetOffsetY = offsetY;
	}

	float		AngleH;				// 水平角度
	float		AngleV;				// 垂直角度
	float		Near;
	float		Far;
	VECTOR		Eye;				// カメラ座標
	VECTOR		Target;				// 注視点座標

	bool isDebugOverView;  // 上視点デバッグフラグ

	float debugDist = 50.0f;
	float debugHeight = 25.0f;
	float debugTargetOffsetY = 0.0f;
};
