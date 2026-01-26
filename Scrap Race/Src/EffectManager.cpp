#include "EffectManager.h"

DamageFlash::DamageFlash(float damage, int r, int g, int b)
	: colorR(r), colorG(g), colorB(b)
{
	timer = 0.3f;  // 表示時間
	intensity = (damage / 50.0f);  // ダメージに応じて強さ調整
	if (intensity > 1.0f) intensity = 1.0f;
}

void DamageFlash::Update(float delta)
{
	if (timer > 0.0f) {
		timer -= delta;
		intensity = timer * 2.0f;  // 徐々に薄く
		if (intensity > 1.0f) intensity = 1.0f;
	}
}

void DamageFlash::Draw()
{
	if (timer <= 0.0f) return;

	if (timer > 0.0f) {
		int screenW = 1280;
		int screenH = 720;

		int edgeSize = 50;  // 赤い枠の太さ
		int baseAlpha = (int)(intensity * 150);

		SetDrawBlendMode(DX_BLENDMODE_ALPHA, baseAlpha);

		// 上
		DrawBox(0, 0, screenW, edgeSize, GetColor(colorR, colorG, colorB), TRUE);

		// 下
		DrawBox(0, screenH - edgeSize, screenW, screenH, GetColor(colorR, colorG, colorB), TRUE);

		// 左
		DrawBox(0, edgeSize, edgeSize, screenH - edgeSize, GetColor(colorR, colorG, colorB), TRUE);

		// 右
		DrawBox(screenW - edgeSize, edgeSize, screenW, screenH - edgeSize, GetColor(colorR, colorG, colorB), TRUE);

		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
	}
}

ScreenShake::ScreenShake(float damage)
{
	timer = 0.5f;  // 0.3秒間揺れる
	intensity = damage * 1.0f;  // ダメージに応じて揺れの強さを調整
	if (intensity > 20.0f) intensity = 20.0f;  // 最大値制限
	offset = VGet(0.0f, 0.0f, 0.0f);
}

void ScreenShake::Update(float delta)
{
	if (timer > 0.0f) {
        timer -= delta;
        
        // ランダムな方向に揺らす
        float currentIntensity = intensity * (timer / 0.3f);  // 徐々に弱く
        
        // ランダムな角度
        float angle = (float)(rand() % 360) * DX_PI_F / 180.0f;
        
        offset.x = cosf(angle) * currentIntensity;
        offset.y = sinf(angle) * currentIntensity;
        offset.z = 0.0f;
    }
    else {
        offset = VGet(0.0f, 0.0f, 0.0f);
    }
}

EffectManager::EffectManager()
{
}

EffectManager::~EffectManager()
{
	Terminate();
}

void EffectManager::Initialize()
{
	Clear();
}

void EffectManager::Terminate()
{
	Clear();
}

void EffectManager::Update(float delta)
{
	// ダメージフラッシュ更新
	for (auto it = damageFlashes.begin(); it != damageFlashes.end(); )
	{
		(*it)->Update(delta);
		if ((*it)->IsFinished())
		{
			delete *it;
			it = damageFlashes.erase(it);
		}
		else
		{
			++it;
		}
	}

	// 画面揺れ更新
	for( auto it = screenShakes.begin(); it != screenShakes.end(); )
	{
		(*it)->Update(delta);
		if ((*it)->IsFinished())
		{
			delete *it;
			it = screenShakes.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void EffectManager::Draw()
{
	for (auto* flash : damageFlashes)
	{
		flash->Draw();
	}
}

void EffectManager::AddDamageFlash(float damage, int r, int g, int b)
{
	damageFlashes.push_back(new DamageFlash(damage, r, g, b));
}

void EffectManager::AddScreenShake(float damage)
{
	screenShakes.push_back(new ScreenShake(damage));
}

VECTOR EffectManager::GetScreenShakeOffset() const
{
	VECTOR totalOffset = VGet(0.0f, 0.0f, 0.0f);

	for (auto* shake : screenShakes)
	{
		VECTOR offset = shake->GetOffset();
		totalOffset = VAdd(totalOffset, offset);
	}

	return totalOffset;
}

void EffectManager::Clear()
{
	for (auto* flash : damageFlashes)
	{
		delete flash;
	}
	damageFlashes.clear();

	for(auto* shake : screenShakes)
	{
		delete shake;
	}
	screenShakes.clear();
}