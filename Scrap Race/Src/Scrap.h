#pragma once
#include "DxLib.h"
#include <vector>
#include "Player.h"

enum class ScrapType {
	Normal,
	Rare
};

class Scrap {
public:
	void Scrap_Initialize(const VECTOR& position, ScrapType scraptype, int normalModel, int rareModel);
	void Scrap_Terminate();

	void Scrap_Update(float deltaTime);
	void Scrap_Draw();
	bool IsExpired() const { return lifetime <= 0.0f || collected; }
	void CheckCollision(Player& player);
private:
	VECTOR pos;
	float lifetime = 10.0f;
	float radius = 2.0f; // Õ“Ë”»’è—p
	float healAmount = 0.0f;
	ScrapType type;
	int modelHandle = -1;
	bool collected = false;
};
