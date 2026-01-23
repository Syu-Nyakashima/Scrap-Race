#pragma once
#include "DxLib.h"
#include <vector>

class CarBase;
class Stage;

class MiniMap
{
public:
	MiniMap(int x, int y, float w, float h, float worldW, float worldH, Stage* stageRef);
	~MiniMap();

	void Initialize();
	void Terminate();

	void Draw(const std::vector<CarBase*>& allCars,int Player);

private:
	void WorldToMiniMap(float worldX, float worldZ, int& miniX, int& miniY);
	void CreateStageTopViewImage();
	void DrawStageLayout();

private:
	int mapX,mapY; 
	float mapWidth, mapHeight;
	float scale;

	float worldWidth, worldHeight;

	int stageTopViewImageHandle;

	// 色定義
	unsigned int bgColor;
	unsigned int frameColor;
	unsigned int playerColor;
	unsigned int enemyColor;
	unsigned int enemyDeadColor;
	unsigned int checkpointColor;

	// スクリーンバッファ
	int mapScreen;

	Stage* stage;

};