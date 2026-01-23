#include "TitleScene.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "MenuScene.h"

TitleScene::TitleScene()
{
}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize()
{
	TitleImage = LoadGraph("Data/Image/Title.png");
}

void TitleScene::Terminate()
{
	DeleteGraph(TitleImage);
}

void TitleScene::Update()
{
	UpdateKeyState();

	if (IsKeyJustPressed(KEY_INPUT_RETURN) || IsKeyJustPressed(KEY_INPUT_SPACE))
	{
		// メニューシーンへ遷移
		SceneManager::ChangeScene(new MenuScene());
	}

	Draw();
}

void TitleScene::Draw()
{
	ClearDrawScreen();
	DrawExtendGraph(0, 0, 1280, 720, TitleImage, true);
	ScreenFlip();
}
