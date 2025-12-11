#include "Result.h"
#include "DxLib.h"
#include "SceneManager.h"
#include "PlayScene.h"
#include "MenuScene.h"

Result::Result(ResultType resultType, const ResultData& resultData) 
    : type(resultType), data(resultData), selectedMenu(0), displayTimer(0)
{
}

Result::~Result()
{
}

void Result::Initialize()
{
	selectedMenu = 0;
	displayTimer = 0;
}

void Result::Terminate()
{
}

void Result::Update()
{
	if (ProcessMessage()) return;

	displayTimer++;

    UpdateKeyState();

	UpdateMenu();

	Draw();
}

void Result::Draw()
{
    SetBackgroundColor(0, 0, 0);
    ClearDrawScreen();

    if (type == RESULT_GAMEOVER){
        DrawGameOver();
    }
    else{
        DrawClear();
    }

    ScreenFlip();
}

void Result::UpdateMenu()
{
    if (IsKeyJustPressed(KEY_INPUT_UP) || IsKeyJustPressed(KEY_INPUT_W))
    {
        selectedMenu--;
        if (selectedMenu < 0) selectedMenu = RESULT_MAX - 1;
    }

    // 下キーで選択を下に
    if (IsKeyJustPressed(KEY_INPUT_DOWN) || IsKeyJustPressed(KEY_INPUT_S))
    {
        selectedMenu++;
        if (selectedMenu >= RESULT_MAX) selectedMenu = 0;
    }

    // 決定キー
    if (IsKeyJustPressed(KEY_INPUT_RETURN) || IsKeyJustPressed(KEY_INPUT_SPACE))
    {
        ExecuteMenu();
    }
}

void Result::DrawGameOver()
{
    // タイトル
    DrawString(400, 100, "GAME OVER", GetColor(255, 0, 0));

    // リザルト情報
    DrawFormatString(400, 200, GetColor(255, 255, 255), "Time: %.2f秒", data.raceTime);
    DrawFormatString(400, 230, GetColor(255, 255, 255), "Final Speed: %.2f", data.finalSpeed);
    DrawFormatString(400, 260, GetColor(255, 255, 255), "Final HP: %.2f", data.finalHp);

    // メニュー
    int menuY = 400;
    const char* menuText[] = { "RETRY", "STAGE SELECT", "MENU", "TITLE" };

    for (int i = 0; i < RESULT_MAX; i++)
    {
        unsigned int color = (i == selectedMenu) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
        DrawString(450, menuY + i * 40, menuText[i], color);
    }

    DrawString(420, menuY + selectedMenu * 40, "→", GetColor(255, 255, 0));
}

void Result::DrawClear()
{
    // タイトル
    DrawString(400, 100, "CLEAR!", GetColor(0, 255, 0));

    // リザルト情報
    DrawFormatString(400, 200, GetColor(255, 255, 255), "Clear Time: %.2f秒", data.raceTime);
    DrawFormatString(400, 230, GetColor(255, 255, 255), "Final Speed: %.2f", data.finalSpeed);
    DrawFormatString(400, 260, GetColor(255, 255, 255), "Final HP: %.2f", data.finalHp);

    // メニュー
    int menuY = 400;
    const char* menuText[] = { "RETRY", "STAGE SELECT", "MENU", "TITLE" };

    for (int i = 0; i < RESULT_MAX; i++)
    {
        unsigned int color = (i == selectedMenu) ? GetColor(255, 255, 0) : GetColor(200, 200, 200);
        DrawString(450, menuY + i * 40, menuText[i], color);
    }

    DrawString(420, menuY + selectedMenu * 40, "→", GetColor(255, 255, 0));
}

void Result::ExecuteMenu()
{
    switch (selectedMenu)
    {
    case RESULT_RETRY:
        SceneManager::ChangeScene(new PlayScene());
        break;

    case RESULT_STAGE_SELECT:
        // SceneManager::ChangeScene(new StageSelect());  // 後で実装
        break;

    case RESULT_MENU:
        SceneManager::ChangeScene(new MenuScene());
        break;

    case RESULT_TITLE:
        // SceneManager::ChangeScene(new TitleScene());  // 後で実装
        break;

    default:
        break;
    }
}
