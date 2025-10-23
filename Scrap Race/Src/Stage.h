#pragma once
#include "Player.h"

class Stage 
{
public:
	void Stage_Initialize();
	void Stage_Terminate();

	void Stage_Update();
private:
	Player player;
};