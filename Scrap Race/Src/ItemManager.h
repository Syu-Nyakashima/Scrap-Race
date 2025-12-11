#pragma once
#include "DxLib.h"
#include <vector>
#include "Scrap.h"
#include "Player.h"

class ItemManager {
public:
	ItemManager();
	~ItemManager();	

	void Initialize();
	void Terminate();

	void Update(const VECTOR& playerPos, float playerAngle, float deltaTime,Player& player,int checkColModel);
	void Draw();

private:
	//Scrap関数
	void SpawnNormalScrap(const VECTOR& playerPos, int checkColModel);
	void SpawnRareScrap(const VECTOR& playerPos, float playerAngle, int checkColModel, int count);

	//衝突判定
	float GetGroundHeight(VECTOR position, int checkColModel);
	bool IsPositionValid(VECTOR position, int checkColModel, float checkRadius);

	//内部処理
	void UpdateScraps(float deltaTime, Player& player, int checkColModel);
	void RemoveExpiredScraps();
	void RemoveOldestScrap();

	//変数
	std::vector<Scrap> Scraps;
	float scrapSpawnTimer;
	float scrapSpawnInterval;
	size_t maxScraps;// 最大出現数

	int normalScrapModel = -1;
	int rareScrapModel = -1;

	bool lastWallHitState = false;

	//定数
    // NormalScrap生成
    static constexpr float NORMAL_SCRAP_SPAWN_INTERVAL = 1.0f;
    static constexpr float NORMAL_MIN_DISTANCE = 50.0f;
    static constexpr float NORMAL_MAX_DISTANCE = 100.0f;
    static constexpr int NORMAL_MAX_SPAWN_ATTEMPTS = 15;
    static constexpr float NORMAL_GROUND_OFFSET = 0.5f;  // 地面から0.5m上

    // RareScrap生成
    static constexpr int RARE_MAX_ATTEMPTS_PER_SCRAP = 5;
    static constexpr float RARE_SPAWN_DISTANCE = 2.0f;   // Playerから2m後ろ
    static constexpr float RARE_SPAWN_HEIGHT = 1.5f;     // 1.5m上
    static constexpr float RARE_FLY_MIN_DISTANCE = 3.0f;
    static constexpr float RARE_FLY_MAX_DISTANCE = 6.0f;
    static constexpr float RARE_INVINCIBLE_TIME = 1.0f;

    // 位置チェック
    static constexpr float SCRAP_CHECK_HEIGHT_TOP = 1.0f;
    static constexpr float SCRAP_CHECK_HEIGHT_BOTTOM = -0.2f;
    static constexpr float WALL_NORMAL_THRESHOLD = 0.5f;
    static constexpr float NORMAL_CHECK_RADIUS = 0.8f;
    static constexpr float RARE_CHECK_RADIUS = 0.5f;

    // 地面高さ範囲
    static constexpr float MIN_GROUND_HEIGHT = -100.0f;
    static constexpr float MAX_GROUND_HEIGHT = 100.0f;

    // Scrap上限
    static constexpr size_t MAX_SCRAPS = 10;
};
