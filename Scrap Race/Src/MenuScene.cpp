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
}

void MenuScene::Terminate() 
{

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
    SetBackgroundColor(0, 0, 0);
    ClearDrawScreen();

    // タイトル
    DrawString(400, 100, "Menu", GetColor(0, 255, 0));

    // メニュー
    int menuY = 400;
    const char* menuText[] = { "SOLO", "MALTI", "TITLE" };

    for (int i = 0; i < MENU_MAX; i++)
    {
        unsigned int color = (i == selectedMenu) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
        DrawString(450, menuY + i * 40, menuText[i], color);
    }

    DrawString(420, menuY + selectedMenu * 40, "→", GetColor(255, 255, 0));

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