#include "Scrap.h"

namespace {
	// 物理パラメータ
	const float GRAVITY = -9.8f;      // 重力
	const float GROUND_FRICTION = 0.9f;  // 地面摩擦
}

void Scrap::Initialize(const VECTOR& position, ScrapType scraptype, int normalModel, int rareModel)
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

void Scrap::Terminate()
{
	
}

void Scrap::Update(float deltaTime, int checkColModel)
{
	lifetime -= deltaTime;

	invincibleTime -= deltaTime;

	vel.y += GRAVITY * deltaTime;

	pos = VAdd(pos, VScale(vel, deltaTime));

	// 地面判定
	if (checkColModel != -1)
	{
		VECTOR lineStart = VAdd(pos, VGet(0.0f, 1.0f, 0.0f));
		VECTOR lineEnd = VAdd(pos, VGet(0.0f, -1.0f, 0.0f));

		MV1_COLL_RESULT_POLY groundHit = MV1CollCheck_Line(
			checkColModel, -1, lineStart, lineEnd
		);

		if (groundHit.HitFlag == 1)
		{
			float groundY = groundHit.HitPosition.y;

			// 地面より下にいたら
			if (pos.y < groundY + 0.3f)
			{
				pos.y = groundY + 0.3f;

				// バウンド（跳ね返る）
				if (vel.y < 0.0f)
				{
					vel.y = -vel.y * 0.4f;  // 40%の力で跳ね返る
				}

				// 地面摩擦
				vel.x *= GROUND_FRICTION;
				vel.z *= GROUND_FRICTION;

				// ほぼ止まったら完全停止
				if (VSize(vel) < 0.5f)
				{
					vel = VGet(0.0f, 0.0f, 0.0f);
				}
			}
		}
	}
}

void Scrap::Draw()
{
	//デバッグ用、あとで消す 
	switch (type)
	{
	case ScrapType::Normal:
		DrawSphere3D(pos, 2.0f, 32, GetColor(255, 0, 0), GetColor(255, 255, 255), FALSE);
		break;
	case ScrapType::Rare:
		DrawSphere3D(pos, 2.0f, 32, GetColor(0, 255, 0), GetColor(255, 255, 255), FALSE);
		break;
	default:
		break;
	}
	

	if (modelHandle < 0) return;
	MV1SetPosition(modelHandle, pos);
	MV1DrawModel(modelHandle);
}

//scrap取得関数
void Scrap::CheckCollision(CarBase& car)
{
	if (invincibleTime > 0.0f) return;

	float dist = VSize(VSub(pos, car.pos));
	if (dist < radius + car.capsuleRadius) {
		collected = true;
		player.Heal(healAmount);
	}
}

void Scrap::SetVelocity(const VECTOR& velocity)
{
	vel = velocity;
}

void Scrap::SetInvincibleTime(float time)
{
	invincibleTime = time;
}
