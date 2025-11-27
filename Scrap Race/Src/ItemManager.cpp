#include "ItemManager.h"


void ItemManager::Initialize()
{
	Scraps.clear();
	scrapSpawnTimer = 0.0f;

	//normalScrapModel = MV1LoadModel("Data/Model/Scrap_Normal.mv1");
	//if (normalScrapModel == -1) printfDx("normalScrapモデル読み込み失敗！\n");
	//rareScrapModel = MV1LoadModel("Data/Model/Scrap_Rare.mv1");
	//if (rareScrapModel == -1) printfDx("rareScrapモデル読み込み失敗！\n");
}

void ItemManager::Terminate()
{
	if (normalScrapModel >= 0) MV1DeleteModel(normalScrapModel);
	if (rareScrapModel >= 0)   MV1DeleteModel(rareScrapModel);
	normalScrapModel = rareScrapModel = -1;

	Scraps.clear();
}

void ItemManager::Update(const VECTOR& playerPos, float playerAngle,float deltaTime,Player& player, int checkColModel)
{
	scrapSpawnTimer += deltaTime;

	//プレイヤーの周りにスクラップ出現
	if (scrapSpawnTimer >= scrapSpawnInterval) {
		SpawnNormalScrap(playerPos, checkColModel);
		scrapSpawnTimer = 0.0f;
	}

	if (player.hitWall&&!lastWallHitState) {

		SpawnRareScrap(playerPos, playerAngle, checkColModel, 3);
	}

	lastWallHitState = player.hitWall;

	//スクラップがある間更新
	for (auto& scrap : Scraps) {
		scrap.Update(deltaTime,checkColModel);
		scrap.CheckCollision(player);
	}

	//時間経過、または取得で消滅
	Scraps.erase(
		std::remove_if(Scraps.begin(), Scraps.end(),
			[](const Scrap& s) { return s.IsExpired(); }),
		Scraps.end()
	);
}

void ItemManager::Draw()
{
	for (auto& scrap : Scraps) {
		scrap.Draw();
	}
}

void ItemManager::SpawnNormalScrap(const VECTOR& playerPos,int checkColModel)
{
	const int MAX_ATTEMPTS = 15;  // 試行回数を増やす
	const float MIN_DISTANCE = 50.0f;   // Player から最低5m
	const float MAX_DISTANCE = 100.0f;  // Player から最大20m

	bool spawned = false;

	for (int attempt = 0; attempt < MAX_ATTEMPTS && !spawned; attempt++)
	{
		// ランダムな位置を生成（Playerの周囲5~20m）
		float angle = ((float)rand() / RAND_MAX) * DX_TWO_PI_F;
		float distance = MIN_DISTANCE + ((float)rand() / RAND_MAX) * (MAX_DISTANCE - MIN_DISTANCE);

		VECTOR offset = VGet(cosf(angle) * distance, 0.0f, sinf(angle) * distance);
		VECTOR spawnPos = VAdd(playerPos, offset);

		// 地面の高さを取得
		float groundY = GetGroundHeight(spawnPos, checkColModel);

		// 地面が見つからない場合はスキップ
		if (groundY < -50.0f || groundY > 50.0f) continue;

		spawnPos.y = groundY + 0.5f;  // 地面から0.5m上

		// 壁に埋まっていないかチェック（小さめの半径でチェック）
		if (IsPositionValid(spawnPos, checkColModel, 0.8f))
		{
			if (Scraps.size() >= maxScraps) {
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
		printfDx("Normal Scrap生成失敗（ % d回試行）\n", MAX_ATTEMPTS);
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
		VECTOR offset = VGet(sinf(angle) * 2.0f, 1.5f, cosf(angle) * 2.0f); //	2m後ろ、1.5m上に生成

		VECTOR spawnPos = VAdd(playerPos, offset);

		// 壁チェック
		if (IsPositionValid(spawnPos, checkColModel, 0.8f))
		{
			if (Scraps.size() >= maxScraps) {
				Scraps.erase(Scraps.begin());
			}

			Scraps.emplace_back();
			Scrap& newScrap = Scraps.back();
			newScrap.Initialize(spawnPos, ScrapType::Rare, normalScrapModel, rareScrapModel);

			// 飛ぶ速さの計算
			float flyDistance = 3.0f + ((float)rand() / RAND_MAX) * 3.0f;
			VECTOR flyDirection = VGet(sinf(angle), 0.3f, cosf(angle));
			flyDirection = VNorm(flyDirection);

			VECTOR velocity = VScale(flyDirection, flyDistance * 3.0f);
			newScrap.SetVelocity(velocity);
			newScrap.SetInvincibleTime(1.0f);

			spawned++;
		}
	}

	printfDx("Rare Scrap×%d 飛び散り生成！\n", spawned);
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
	VECTOR capsuleTop = VAdd(position, VGet(0.0f, 1.0f, 0.0f));      // 上
	VECTOR capsuleBottom = VAdd(position, VGet(0.0f, -0.2f, 0.0f));  // 少し下（地面に接触しないように）

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
		if (fabsf(normal.y) < 0.5f)  // Y成分が小さい = 横向き
		{
			hasWallCollision = true;
			break;
		}
	}

	MV1CollResultPolyDimTerminate(hitResult);

	// 壁がない = 有効(true)
	return !hasWallCollision;
}
