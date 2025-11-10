#include "Scrap.h"

void Scrap::Scrap_Initialize(const VECTOR& position, ScrapType scraptype, int normalModel, int rareModel)
{
	pos = position;
	lifetime=10.0f;
	type = scraptype;

	//type別回復量変化
	switch (type) {
	case ScrapType::Normal: healAmount = 5.0f; break;
	case ScrapType::Rare:   healAmount = 10.0f; break;
	}

	//type別モデル変化
	modelHandle = (type == ScrapType::Normal) ? normalModel : rareModel;

}

void Scrap::Scrap_Terminate()
{
	
}

void Scrap::Scrap_Update(float deltaTime)
{
	lifetime -= deltaTime;
}

void Scrap::Scrap_Draw()
{
	//デバッグ用、あとで消す
	DrawSphere3D(pos, 2.0f, 32, GetColor(255, 0, 0), GetColor(255, 255, 255),FALSE);

	if (modelHandle < 0) return;
	MV1SetPosition(modelHandle, pos);
	MV1DrawModel(modelHandle);
}

//scrap取得関数
void Scrap::CheckCollision(Player& player)
{
	float dist = VSize(VSub(pos, player.pos));
	if (dist < radius + player.radius) {
		collected = true;
		player.Heal(healAmount);
	}
}
