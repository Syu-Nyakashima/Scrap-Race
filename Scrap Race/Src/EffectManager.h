#pragma once
#include "DxLib.h"
#include <vector>

class DamageFlash
{
public:
	float timer;
	float intensity;
	int colorR, colorG, colorB;

	DamageFlash(float damage, int r = 255, int g = 0, int b = 0);
	void Update(float delta);
	void Draw();
	bool IsFinished() const { return timer <= 0.0f; }
};

class ScreenShake
{
public:
	float timer;
	float intensity;
	VECTOR offset;  // 揺れのオフセット

	ScreenShake(float damage);
	void Update(float delta);
	VECTOR GetOffset() const { return offset; }
	bool IsFinished() const { return timer <= 0.0f; }
};

class EffectManager
{
public:
	EffectManager();
	~EffectManager();
	void Initialize();
	void Terminate();
	void Update(float delta);
	void Draw();

	// ダメージフラッシュ
	void AddDamageFlash(float damage, int r = 255, int g = 0, int b = 0);

	// 画面揺れ
	void AddScreenShake(float damage);

	// 画面揺れオフセット取得
	VECTOR GetScreenShakeOffset() const;

	// 全エフェクトをクリア
	void Clear();
private:
	std::vector<DamageFlash*> damageFlashes;
	std::vector<ScreenShake*> screenShakes;
};