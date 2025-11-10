#include "ItemManager.h"


void ItemManager::ItemManager_Initialize()
{
	Scraps.clear();
	scrapSpawnTimer = 0.0f;

	//normalScrapModel = MV1LoadModel("Data/Model/Scrap_Normal.mv1");
	//if (normalScrapModel == -1) printfDx("normalScrapモデル読み込み失敗！\n");
	//rareScrapModel = MV1LoadModel("Data/Model/Scrap_Rare.mv1");
	//if (rareScrapModel == -1) printfDx("rareScrapモデル読み込み失敗！\n");
}

void ItemManager::ItemManager_Terminate()
{
	if (normalScrapModel >= 0) MV1DeleteModel(normalScrapModel);
	if (rareScrapModel >= 0)   MV1DeleteModel(rareScrapModel);
	normalScrapModel = rareScrapModel = -1;

	Scraps.clear();
}

void ItemManager::ItemManager_Update(const VECTOR& playerPos, float deltaTime,Player& player)
{
	scrapSpawnTimer += deltaTime;

	//プレイヤーの周りにスクラップ出現
	if (scrapSpawnTimer >= scrapSpawnInterval) {
		SpawnScrapAroundPlayer(playerPos, ScrapType::Normal);
		scrapSpawnTimer = 0.0f;
	}

	//スクラップがある間更新
	for (auto& scrap : Scraps) {
		scrap.Scrap_Update(deltaTime);
		scrap.CheckCollision(player);
	}

	//時間経過、または取得で消滅
	Scraps.erase(
		std::remove_if(Scraps.begin(), Scraps.end(),
			[](const Scrap& s) { return s.IsExpired(); }),
		Scraps.end()
	);
}

void ItemManager::ItemManager_Draw()
{
	for (auto& scrap : Scraps) {
		scrap.Scrap_Draw();
	}
}

void ItemManager::SpawnScrapAroundPlayer(const VECTOR& playerPos, ScrapType type)
{
	float angle = ((float)rand() / RAND_MAX) * DX_TWO_PI_F;
	float dist = scrapSpawnRadius * (0.5f + ((float)rand() / RAND_MAX) * 0.5f);
	VECTOR offset = VGet(cosf(angle) * dist, 0.0f, sinf(angle) * dist);
	VECTOR spawnPos = VAdd(playerPos, offset);

	Scrap newScrap;
	newScrap.Scrap_Initialize(spawnPos, type, normalScrapModel, rareScrapModel);
	
	if (Scraps.size() > maxScraps) {
		Scraps.erase(Scraps.begin());
	}

	Scraps.push_back(newScrap);


}
