#pragma once
#include "DxLib.h"
#include "Player.h"

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

	bool IsExpired() const { return lifetime <= 0.0f || collected; }
	void CheckCollision(std::vector<CarBase*>& cars);
	void SetVelocity(const VECTOR& velocity);
	void SetInvincibleTime(float time);
private:
	VECTOR pos;
	VECTOR vel;

	float lifetime = 10.0f;
	float invincibleTime = 0.0f;
	float radius = 2.0f; // è’ìÀîªíËóp
	float healAmount = 0.0f;
	ScrapType type;
	int modelHandle = -1;
	bool collected = false;
};
