#pragma once
#include "BaseScene.h"
#include "EnemyCPU.h"

enum MenuSelect {
	MENU_SOLO,         // リトライ(同じステージ)
	MENU_MULTI,  // ステージ選択
	MENU_TITLE,         // タイトルに戻る
	MENU_MAX
};

enum DifficultySelect {
	DIFFICULTY_EASY,
	DIFFICULTY_NORMAL,
	DIFFICULTY_HARD,
	DIFFICULTY_MAX
};

class MenuScene : public BaseScene {
public:
	MenuScene();
	~MenuScene();

	void Initialize() override;
	void Terminate() override;
	void Update() override;

private:
	void Draw();			// 描画処理
	void UpdateMenu();      // メニュー選択更新
	void UpdateDifficultySelect();  // 難易度選択更新
	void ExecuteMenu();     // メニュー実行
	void StartGame();

private:
	int selectedMenu;
	int selectedDifficulty;

	int MenuImage;		  // メニュー画面の背景
	int MenuSelectFrame;  // メニュー選択のフレーム
	int DifficultyImage;  // 難易度選択画面の背景
	int DifficultyFrame;  // 難易度選択のフレーム

	bool isSelectingDifficulty;  // 難易度選択中かどうか
	bool isEnding = false;
};