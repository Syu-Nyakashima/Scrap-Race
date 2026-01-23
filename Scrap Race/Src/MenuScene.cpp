#include "MenuScene.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "PlayScene.h"
#include "TitleScene.h"

MenuScene::MenuScene()
{
}

MenuScene::~MenuScene()
{
}

void MenuScene::Initialize() 
{
	selectedMenu = 0;
    selectedDifficulty = 0;
    isSelectingDifficulty = false;

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
    UpdateKeyState();

    if (isSelectingDifficulty)
    {
        UpdateDifficultySelect();
    }
    else
    {
        UpdateMenu();
	}
    
    Draw();
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

void MenuScene::UpdateDifficultySelect()
{
    if (IsKeyJustPressed(KEY_INPUT_UP) || IsKeyJustPressed(KEY_INPUT_W))
    {
        selectedDifficulty--;
        if (selectedDifficulty < 0) selectedDifficulty = DIFFICULTY_MAX - 1;
    }

    if (IsKeyJustPressed(KEY_INPUT_DOWN) || IsKeyJustPressed(KEY_INPUT_S))
    {
        selectedDifficulty++;
        if (selectedDifficulty >= DIFFICULTY_MAX) selectedDifficulty = 0;
    }

    if (IsKeyJustPressed(KEY_INPUT_RETURN) || IsKeyJustPressed(KEY_INPUT_SPACE))
    {
        StartGame();
    }

    if (IsKeyJustPressed(KEY_INPUT_ESCAPE))
    {
        isSelectingDifficulty = false;
    }
}

void MenuScene::ExecuteMenu() 
{
    switch (selectedMenu)
    {
    case MENU_SOLO:
        isSelectingDifficulty = true;
        break;

    case MENU_MULTI:

        break;

    case MENU_TITLE:
        SceneManager::ChangeScene(new TitleScene());  // 後で実装
        break;

    default:
        break;
    }
}

void MenuScene::StartGame()
{
    AIDifficulty difficulty;

    switch (selectedDifficulty)
    {
    case DIFFICULTY_EASY:
        difficulty = AIDifficulty::Easy;
        break;
    case DIFFICULTY_NORMAL:
        difficulty = AIDifficulty::Normal;
        break;
    case DIFFICULTY_HARD:
        difficulty = AIDifficulty::Hard;
        break;
    default:
        difficulty = AIDifficulty::Normal;
        break;
    }

    // PlaySceneに難易度を渡す
    SceneManager::ChangeScene(new PlayScene(difficulty));
}

void MenuScene::Draw()
{
    ClearDrawScreen();
    if(isSelectingDifficulty)
    {
        // 背景画像がない場合は暗い背景
        DrawBox(0, 0, 1280, 720, GetColor(20, 20, 40), TRUE);

        // タイトル
        SetFontSize(64);
        DrawString(400, 100, "難易度選択", GetColor(255, 255, 255));

        // 難易度オプション
        SetFontSize(48);
        int baseY = 250;
        int spacing = 150;

        // Easy
        unsigned int easyColor = (selectedDifficulty == DIFFICULTY_EASY)
            ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
        DrawString(500, baseY, "EASY", easyColor);
        DrawString(700, baseY + 10, "- 初心者向け", easyColor);

        // Normal
        unsigned int normalColor = (selectedDifficulty == DIFFICULTY_NORMAL)
            ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
        DrawString(500, baseY + spacing, "NORMAL", normalColor);
        DrawString(700, baseY + spacing + 10, "- 標準", normalColor);

        // Hard
        unsigned int hardColor = (selectedDifficulty == DIFFICULTY_HARD)
            ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
        DrawString(500, baseY + spacing * 2, "HARD", hardColor);
        DrawString(700, baseY + spacing * 2 + 10, "- 上級者向け", hardColor);

        // 選択カーソル
        DrawString(450, baseY + spacing * selectedDifficulty, "→", GetColor(255, 255, 0));

        // 操作説明
        SetFontSize(24);
        DrawString(400, 650, "↑↓:選択  Enter:決定  ESC:戻る", GetColor(255, 255, 255));
	}
    else 
    {
        DrawExtendGraph(0, 0, 1280, 720, MenuImage, true);

        DrawExtendGraph(selectedMenu * 37, selectedMenu * 140, selectedMenu * 37 + 1280, selectedMenu * 140 + 720, MenuSelectFrame, true);
    }
    
    ScreenFlip();
}