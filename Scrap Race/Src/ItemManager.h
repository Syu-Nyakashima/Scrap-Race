#pragma once
#include "DxLib.h"
#include <vector>
#include "Scrap.h"
#include "Player.h"

class ItemManager {
public:
	void Initialize();
	void Terminate();

	void Update(const VECTOR& playerPos, float playerAngle, float deltaTime,Player& player,int checkColModel);
	void Draw();

private:
	//Scrapä÷êî

	void SpawnNormalScrap(const VECTOR& playerPos, int checkColModel);
	void SpawnRareScrap(const VECTOR& playerPos, float playerAngle, int checkColModel, int count);

	//è’ìÀîªíË
	float GetGroundHeight(VECTOR position, int checkColModel);
	bool IsPositionValid(VECTOR position, int checkColModel, float checkRadius);

	//ïœêî
	std::vector<Scrap> Scraps;
	float scrapSpawnTimer = 0.0f;
	float scrapSpawnInterval = 1.0f;
	float scrapSpawnRadius = 50.0f;
	size_t maxScraps = 10;// ç≈ëÂèoåªêî

	int normalScrapModel = -1;
	int rareScrapModel = -1;

	bool lastWallHitState = false;
};
