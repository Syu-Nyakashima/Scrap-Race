#pragma once
#include "DxLib.h"
#include <vector>

class Stage 
{
public:
	void Initialize();
	void Terminate();

	void Update();
	void Draw();

	int GetCheckColModel() const { return CheckColModel; }

	bool IsInsideCheckpoint(VECTOR carPos, int currentCheckpoint);
	int GetTotalCheckpoints() const { return (int)checkpoints.size(); }
	VECTOR GetCheckpointPos(int index) const;
	void DrawCheckpoints() const;

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

	struct Checkpoint
	{
		VECTOR pos;
		float width;
		float height;
		float depth;
	};

	std::vector<Checkpoint> checkpoints;
};