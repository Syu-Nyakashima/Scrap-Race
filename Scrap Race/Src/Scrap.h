#pragma once
#include "DxLib.h"

class CarBase;

enum class ScrapType {
	Normal,
	Rare
};

class Scrap {
public:
	void Initialize(const VECTOR& position, ScrapType scraptype, int normalModel, int rareModel);
	void Terminate();

	void Update(float deltaTime,int checkColModel);
	void Draw();

	void CheckCollision(CarBase& car);

	void SetVelocity(const VECTOR& velocity);
	void SetInvincibleTime(float time);
	void ApplyMagnetism(const VECTOR& carPos, float deltaTime);

	bool IsExpired() const { return lifetime <= 0.0f || collected; }
	bool IsCollected() const { return collected; }

	VECTOR GetPosition() const { return pos; }
private:
	VECTOR pos;
	VECTOR vel;

	float lifetime = 10.0f; 
	float invincibleTime = 0.0f; //生成後の無敵時間
	float radius = 2.0f; // 衝突判定用
	float healAmount = 5.0f;

	ScrapType type;
	int modelHandle = -1;
	bool collected = false;
	bool isBeingMagnetized = false; // 引き寄せ中フラグ

	//ステータス回復
	float spdMaxBoost = 5.0f;
	float spdUpBoost = 0.05f;
	float spdDownBoost = 0.05f;

	static constexpr float magnetRange = 50.0f;      // 引き寄せ開始距離
	static constexpr float magnetForce = 150.0f;       // 引き寄せの強さ
};
