#include "MenuScene.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "PlayScene.h"

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

void MenuScene::Initialize() 
{
	selectedMenu = 0;

    MenuImage = LoadGraph("Data/Image/Menu_1.png");
    MenuSelectFrame = LoadGraph("Data/Image/MenuFrame.png");
}

void MenuScene::Terminate() 
{
    DeleteGraph(MenuImage);
    DeleteGraph(MenuSelectFrame);
}

void MenuScene::Update() 
{
	if (ProcessMessage()) return;
    
    UpdateKeyState();

	UpdateMenu();
    
    Draw();
}

void MenuScene::Draw() 
{
    ClearDrawScreen();

    DrawExtendGraph(0, 0, 1280, 720, MenuImage, true);

    DrawExtendGraph(selectedMenu * 37, selectedMenu * 140, selectedMenu * 37 + 1280, selectedMenu * 140 + 720, MenuSelectFrame, true);

    ScreenFlip();
}

void MenuScene::UpdateMenu() 
{
    if (IsKeyJustPressed(KEY_INPUT_UP) || IsKeyJustPressed(KEY_INPUT_W))
    {
        selectedMenu--;
        if (selectedMenu < 0) selectedMenu = MENU_MAX - 1;
    }

    // 下キーで選択を下に
    if (IsKeyJustPressed(KEY_INPUT_DOWN) || IsKeyJustPressed(KEY_INPUT_S))
    {
        selectedMenu++;
        if (selectedMenu >= MENU_MAX) selectedMenu = 0;
    }

    // 決定キー
    if (IsKeyJustPressed(KEY_INPUT_RETURN) || IsKeyJustPressed(KEY_INPUT_SPACE))
    {
        ExecuteMenu();
    }
}

void MenuScene::ExecuteMenu() 
{
    switch (selectedMenu)
    {
    case MENU_SOLO:
        SceneManager::ChangeScene(new PlayScene());
        break;

    case MENU_MULTI:

        break;

    case MENU_TITLE:
        // SceneManager::ChangeScene(new TitleScene());  // 後で実装
        break;

    default:
        break;
    }
}