#pragma once
#include "BaseScene.h"

enum MenuSelect {
	MENU_SOLO,         // リトライ(同じステージ)
	MENU_MULTI,  // ステージ選択
	MENU_TITLE,         // タイトルに戻る
	MENU_MAX
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
	void ExecuteMenu();     // メニュー実行

private:
	int selectedMenu;

	int MenuImage;
	int MenuSelectFrame;

	bool isEnding = false;
};