#pragma once
#include "DxLib.h"
#include "Player.h"

class Camera {
public:
	void Initialize();
	void Terminate();
	void Update(const Player& player, float delta);

	void SetDebugCameraParams(float dist, float height, float offsetY,float Near,float Far) {
		debugDist = dist;
		debugHeight = height;
		debugTargetOffsetY = offsetY;
		debugNear = Near;
		debugFar = Far;
	}

	float		AngleH;				// 水平角度
	float		AngleV;				// 垂直角度
	float		Near;
	float		Far;
	VECTOR		Eye;				// カメラ座標
	VECTOR		Target;				// 注視点座標

	VECTOR smoothEye;
	VECTOR smoothTarget;

	static constexpr float CAMERA_LERP_SPEED = 7.0f;

	bool isDebugOverView;  // 上視点デバッグフラグ

	float debugDist = 15.0f;
	float debugHeight = 0.0f;
	float debugTargetOffsetY = 0.0f;
	float debugNear = 0.1f;
	float debugFar=500.0f;
};
