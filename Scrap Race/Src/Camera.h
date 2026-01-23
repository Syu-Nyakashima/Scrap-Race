#pragma once
#include "DxLib.h"
#include "Player.h"

class Camera {
public:
	void Initialize();
	void Terminate();
	void Update(const Player& player, float delta);

	float		AngleH;				// 水平角度
	float		AngleV;				// 垂直角度
	VECTOR		Eye;				// カメラ座標
	VECTOR		Target;				// 注視点座標

	VECTOR smoothEye;
	VECTOR smoothTarget;

	
private:
	float Dist = 15.0f;
	float Height = 4.0f;
	float TargetOffsetY = 0.0f;
	float Near = 0.1f;
	float Far=500.0f;
	static constexpr float CAMERA_LERP_SPEED = 7.0f;
};
