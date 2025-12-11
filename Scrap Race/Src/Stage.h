#pragma once
#include "DxLib.h"

class Stage 
{
public:
	void Initialize();
	void Terminate();

	void Update();
	void Draw();

	bool CheckGoal(VECTOR playerPos);

	int GetCheckColModel() const { return CheckColModel; }

private:
	int StageModelHandle = -1;
	int CheckColModel = -1;
	int GoalModelHandle = -1;

	VECTOR GoalPos;
	float GoalWidth;
	float GoalHeight;
	float GoalDepth;
	bool isGoal;
	bool wasInside;
	float inZ;
	float outZ;
};