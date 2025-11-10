#pragma once
#include "Player.h"
#include "DxLib.h"

class Stage 
{
public:
	void Stage_Initialize();
	void Stage_Terminate();

	void Stage_Update();
	void Stage_Draw();

	bool CheckGoal(VECTOR playerPos);
private:
	int StageModelHandle;
	int GoalModelHandle;

	VECTOR GoalPos;
	float GoalWidth;
	float GoalHeight;
	float GoalDepth;
	bool isGoal;

	Player player;
};