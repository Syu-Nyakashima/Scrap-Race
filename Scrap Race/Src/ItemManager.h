#pragma once
#include "DxLib.h"
#include <vector>
#include "Scrap.h"
#include "Player.h"

class ItemManager {
public:
	void ItemManager_Initialize();
	void ItemManager_Terminate();

	void ItemManager_Update(const VECTOR& playerPos, float deltaTime,Player& player);
	void ItemManager_Draw();

private:
	//Scrapïœêî
	void SpawnScrapAroundPlayer(const VECTOR& playerPos, ScrapType type);
	std::vector<Scrap> Scraps;
	float scrapSpawnTimer = 0.0f;
	float scrapSpawnInterval = 1.0f;
	float scrapSpawnRadius = 50.0f;
	size_t maxScraps = 10;

	int normalScrapModel = -1;
	int rareScrapModel = -1;
};
