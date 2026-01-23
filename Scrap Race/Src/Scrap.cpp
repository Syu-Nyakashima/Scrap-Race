#include "Scrap.h"
#include "CarBase.h"

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
	collected = false;
	invincibleTime = 0.0f;
	isBeingMagnetized = false;

	//type別回復量変化
	switch (type) {
	case ScrapType::Normal: 
		healAmount = 5.0f; 
		spdMaxBoost = 5.0f;
		spdUpBoost = 0.05f;
		break;
	case ScrapType::Rare:
		healAmount = 15.0f;
		spdMaxBoost = 10.0f;
		spdUpBoost = 0.1f;
		break;
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

	if (!isBeingMagnetized)
	{
		vel.y += GRAVITY * deltaTime;
	}

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
	// 引き寄せフラグをリセット（次フレームで再判定）
	isBeingMagnetized = false;
}

void Scrap::Draw()
{
	if (modelHandle < 0) return;
	MV1SetPosition(modelHandle, pos);
	MV1DrawModel(modelHandle);
}

//scrap取得関数
void Scrap::CheckCollision(CarBase& car)
{
	if (invincibleTime > 0.0f) return;
	if (collected) return;

	float dist = VSize(VSub(pos, car.pos));
	if (dist < radius + car.capsuleRadius) 
	{
		collected = true;
		car.Heal(healAmount);

		// 最高速度UP
		car.BoostStatus(spdMaxBoost, 0.0f);
		//printfDx("Scrap取得！ HP+%.1f, SpdMax+%.1f\n", healAmount, spdMaxBoost);
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

void Scrap::ApplyMagnetism(const VECTOR& carPos, float deltaTime)
{
	if (collected) return;
	if (invincibleTime > 0.0f) return;

	VECTOR toCar = VSub(carPos, pos);
	float dist = VSize(toCar);

	// 引き寄せ範囲内かチェック
	if (dist < magnetRange)
	{
		isBeingMagnetized = true;

		// 正規化して方向ベクトルを取得
		VECTOR direction = VNorm(toCar);

		// 距離に応じて引き寄せ力を調整（近いほど強く）
		float forceMult = 1.0f - (dist / magnetRange);
		forceMult = forceMult * forceMult; // 二乗で非線形に

		// 引き寄せ速度を計算
		VECTOR magnetVel = VScale(direction, magnetForce * forceMult);

		// 既存の速度に引き寄せ速度を加算（徐々に加速）
		vel = magnetVel;

		// 速度制限（速すぎないように）
		float speed = VSize(vel);
		float maxSpeed = magnetForce * 1.5f;
		if (speed > maxSpeed)
		{
			vel = VScale(VNorm(vel), maxSpeed);
		}
	}
}