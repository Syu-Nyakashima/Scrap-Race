#include "ItemManager.h"


ItemManager::ItemManager() 
	: scrapSpawnTimer(0.0f),scrapSpawnInterval(NORMAL_SCRAP_SPAWN_INTERVAL),
	  maxScraps(MAX_SCRAPS),normalScrapModel(-1),rareScrapModel(-1)
{
}

ItemManager::~ItemManager()
{
	Terminate();
}

void ItemManager::Initialize()
{
	Scraps.clear();
	scrapSpawnTimer = 0.0f;
	scrapSpawnInterval = NORMAL_SCRAP_SPAWN_INTERVAL;
	maxScraps = MAX_SCRAPS;

	normalScrapModel = MV1LoadModel("Data/Model/NormalScrap.mv1");
	if (normalScrapModel == -1) printfDx("normalScrapモデル読み込み失敗！\n");
	MV1SetScale(normalScrapModel, VGet(0.05f, 0.05f, 0.05f));

	rareScrapModel = MV1LoadModel("Data/Model/RareScrap.mv1");
	if (rareScrapModel == -1) printfDx("rareScrapモデル読み込み失敗！\n");
	MV1SetScale(rareScrapModel, VGet(0.05f, 0.05f, 0.05f));
}

void ItemManager::Terminate()
{
	if (normalScrapModel >= 0) MV1DeleteModel(normalScrapModel);
	if (rareScrapModel >= 0)   MV1DeleteModel(rareScrapModel);
	normalScrapModel = rareScrapModel = -1;

	Scraps.clear();
}

void ItemManager::Update(float deltaTime, int checkColModel, std::vector<CarBase*>& cars)
{
	if (cars.empty()) return;

	//タイマー更新
	scrapSpawnTimer += deltaTime;

	//プレイヤーの周りにスクラップ出現
	if (scrapSpawnTimer >= scrapSpawnInterval) {

		for (int i = 0; i < cars.size(); i++) {
			SpawnNormalScrap(cars[i]->GetPosition(), checkColModel);
		}
	
		scrapSpawnTimer = 0.0f;
	}

	// 壁衝突でRareスクラップ生成
	CheckCarWallHits(cars, checkColModel);

	//スクラップがある間更新
	for (auto& scrap : Scraps) {
		for (auto* car : cars) {
			if (car != nullptr && car->IsAlive()) {
				scrap.ApplyMagnetism(car->GetPosition(), deltaTime);
			}
		}
		scrap.Update(deltaTime, checkColModel);
	}
	for(auto& rareScrap : RareScraps) {
		for (auto* car : cars) {
			if (car != nullptr && car->IsAlive()) {
				rareScrap.ApplyMagnetism(car->GetPosition(), deltaTime);
			}
		}
		rareScrap.Update(deltaTime, checkColModel);
	}

	CheckAllCollisions(cars);

	//時間経過、または取得で消滅
	Scraps.erase(
		std::remove_if(Scraps.begin(), Scraps.end(),
			[](const Scrap& s) { return s.IsExpired(); }),
		Scraps.end()
	);

	RareScraps.erase(
		std::remove_if(RareScraps.begin(), RareScraps.end(),
			[](const Scrap& s) { return s.IsExpired(); }),
		RareScraps.end()
	);
}

void ItemManager::CheckAllCollisions(std::vector<CarBase*>& cars)
{
	for (auto& scrap : Scraps) {
		for (auto* car : cars)
		{
			if (car != nullptr && car->IsAlive())
			{
				scrap.CheckCollision(*car);
			}
		}
	}

	for(auto& rareScrap : RareScraps) {
		for (auto* car : cars)
		{
			if (car != nullptr && car->IsAlive())
			{
				rareScrap.CheckCollision(*car);
			}
		}
	}
}

void ItemManager::CheckCarWallHits(std::vector<CarBase*>& cars, int checkColModel)
{
	for (auto* car : cars)
	{
		if (car->JustHitWall())
		{
			SpawnRareScrap(car->GetPosition(),car->GetAngle(),checkColModel,3);
		}
	}
}

void ItemManager::Draw()
{
	for (auto& scrap : Scraps) {
		scrap.Draw();
	}

	for(auto& rareScrap : RareScraps) {
		rareScrap.Draw();
	}
}

bool ItemManager::FindNearestScrap(const VECTOR& pos, float searchRadius, VECTOR& outScrapPos)
{
	float nearestDist = searchRadius;
	bool found = false;

	for (const auto& scrap : Scraps)
	{
		if (scrap.IsCollected()) continue;

		float dist = VSize(VSub(scrap.GetPosition(), pos));
		if (dist < nearestDist)
		{
			nearestDist = dist;
			outScrapPos = scrap.GetPosition();
			found = true;
		}
	}

	for(const auto& rareScrap : RareScraps) {
		if (rareScrap.IsCollected()) continue;

		float dist = VSize(VSub(rareScrap.GetPosition(), pos));
		if (dist < nearestDist)
		{
			nearestDist = dist;
			outScrapPos = rareScrap.GetPosition();
			found = true;
		}
	}

	return found;
}

void ItemManager::SpawnNormalScrap(const VECTOR& playerPos,int checkColModel)
{
	bool spawned = false;

	for (int attempt = 0; attempt < NORMAL_MAX_SPAWN_ATTEMPTS && !spawned; attempt++)
	{
		// ランダムな位置を生成（Playerの周囲5~20m）
		float angle = ((float)rand() / RAND_MAX) * DX_TWO_PI_F;
		float distance = NORMAL_MIN_DISTANCE + ((float)rand() / RAND_MAX) * (NORMAL_MAX_DISTANCE - NORMAL_MIN_DISTANCE);

		VECTOR offset = VGet(cosf(angle) * distance, 0.0f, sinf(angle) * distance);
		VECTOR spawnPos = VAdd(playerPos, offset);

		// 地面の高さを取得
		float groundY = GetGroundHeight(spawnPos, checkColModel);

		// 地面が見つからない場合はスキップ
		if (groundY < MIN_GROUND_HEIGHT || groundY > MAX_GROUND_HEIGHT) continue;

		spawnPos.y = groundY + NORMAL_GROUND_OFFSET;  // 地面から0.5m上

		// 壁に埋まっていないかチェック（小さめの半径でチェック）
		if (IsPositionValid(spawnPos, checkColModel, 0.8f))
		{
			if (Scraps.size() >= MAX_SCRAPS) {
				Scraps.erase(Scraps.begin());
			}

			Scraps.emplace_back();
			Scrap& newScrap = Scraps.back();
			newScrap.Initialize(spawnPos, ScrapType::Normal, normalScrapModel, rareScrapModel);

			spawned = true;
		}
	}

	if (!spawned)
	{
		printfDx("Normal Scrap生成失敗（ % d回試行）\n", NORMAL_MAX_SPAWN_ATTEMPTS);
	}
}

void ItemManager::SpawnRareScrap(const VECTOR& playerPos, float playerAngle, int checkColModel, int count)
{
	// Playerの進行方向の逆（跳ね返る方向）
	float rad = playerAngle * DX_PI_F / 180.0f;
	VECTOR backward = VGet(-sinf(rad), 0.0f, -cosf(rad));

	int spawned = 0;

	for (int i = 0; i < count * 2 && spawned < count; i++)  // 試行回数を2倍に
	{
		// ランダムな角度で飛び散る
		float spreadAngle = ((float)rand() / RAND_MAX - 0.5f) * DX_PI_F * 0.5f;  // ±45
		float angle = atan2f(backward.x, backward.z) + spreadAngle;


		VECTOR offset = VGet(sinf(angle) * RARE_SPAWN_DISTANCE,
							RARE_SPAWN_HEIGHT,
							cosf(angle) * RARE_SPAWN_DISTANCE); //	 RARE_SPAWN_DISTANCE分後ろ、RARE_SPAWN_HEIGHT分上に生成
		VECTOR spawnPos = VAdd(playerPos, offset);

		// 壁チェック
		if (IsPositionValid(spawnPos, checkColModel, 0.8f))
		{
			if (RareScraps.size() >= MAX_SCRAPS_RARE) {
				RareScraps.erase(RareScraps.begin());
			}

			RareScraps.emplace_back();
			Scrap& newScrap = RareScraps.back();
			newScrap.Initialize(spawnPos, ScrapType::Rare, normalScrapModel, rareScrapModel);

			// 飛ぶ速さの計算
			float flyDistance = 3.0f + ((float)rand() / RAND_MAX) * (RARE_FLY_MAX_DISTANCE - RARE_FLY_MIN_DISTANCE);
			VECTOR flyDirection = VGet(sinf(angle), 0.3f, cosf(angle));
			flyDirection = VNorm(flyDirection);

			VECTOR velocity = VScale(flyDirection, flyDistance * 3.0f);
			newScrap.SetVelocity(velocity);
			newScrap.SetInvincibleTime(1.0f);

			spawned++;
			//printfDx("Rare Scrap生成 成功 (%d/%d)\n", spawned, count);
		}
	}
}

float ItemManager::GetGroundHeight(VECTOR position, int checkColModel)
{
	if(checkColModel == -1) return 0.0f;

	VECTOR lineStart = VAdd(position, VGet(0.0f, 100.0f, 0.0f));
	VECTOR lineEnd = VAdd(position, VGet(0.0f, -100.0f, 0.0f));

	MV1_COLL_RESULT_POLY hitResult = MV1CollCheck_Line(
		checkColModel, -1, lineStart, lineEnd
	);

	if (hitResult.HitFlag == 1)
	{
		return hitResult.HitPosition.y;
	}

	return position.y;
}

bool ItemManager::IsPositionValid(VECTOR position, int checkColModel, float checkRadius)
{
	if (checkColModel == -1) return true;

	// 横向きのカプセルで壁だけチェック
	VECTOR capsuleTop = VAdd(position, VGet(0.0f, SCRAP_CHECK_HEIGHT_TOP, 0.0f));      // 上
	VECTOR capsuleBottom = VAdd(position, VGet(0.0f, WALL_NORMAL_THRESHOLD, 0.0f));  // 少し下（地面に接触しないように）

	MV1_COLL_RESULT_POLY_DIM hitResult = MV1CollCheck_Capsule(
		checkColModel, -1,
		capsuleTop,
		capsuleBottom,
		checkRadius
	);

	bool hasWallCollision = false;

	// 衝突したポリゴンをチェック
	for (int i = 0; i < hitResult.HitNum; i++)
	{
		VECTOR normal = hitResult.Dim[i].Normal;

		// 法線が横向き = 壁
		if (fabsf(normal.y) < WALL_NORMAL_THRESHOLD)  // Y成分が小さい = 横向き
		{
			hasWallCollision = true;
			break;
		}
	}

	MV1CollResultPolyDimTerminate(hitResult);

	// 壁がない = 有効(true)
	return !hasWallCollision;
}
