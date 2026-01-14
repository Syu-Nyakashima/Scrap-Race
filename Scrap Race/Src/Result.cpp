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
    if(type == RESULT_GAMEOVER){
        ResultImage = LoadGraph("Data/Image/GameOverResult.png");
    }
    else{
        ResultImage = LoadGraph("Data/Image/ClearResult.png");
	}

	LoadDivGraph("Data/Image/RankNumber.png", 10, 10, 1, 100, 90, RankUI);
	LoadDivGraph("Data/Image/ResultMenuButton.png", 2, 2, 1, 400, 98, MenuButtonImage);
	LoadDivGraph("Data/Image/ResultRetryButton.png", 2, 2, 1, 400, 98, RetryButtonImage);

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
    if (IsKeyJustPressed(KEY_INPUT_LEFT) || IsKeyJustPressed(KEY_INPUT_A))
    {
        selectedMenu--;
        if (selectedMenu < 0) selectedMenu = RESULT_MAX - 1;
    }

    // 下キーで選択を下に
    if (IsKeyJustPressed(KEY_INPUT_RIGHT) || IsKeyJustPressed(KEY_INPUT_D))
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
    DrawExtendGraph(0, 0, 1280, 720, ResultImage, true);

	DrawGraph(400, 600, RetryButtonImage[selectedMenu], true);
	DrawGraph(800, 600, MenuButtonImage[selectedMenu], true);
}

void Result::DrawClear()
{
    DrawExtendGraph(0, 0, 1280, 720, ResultImage, true);

    DrawGraph(400, 600, RetryButtonImage[selectedMenu], true);
    DrawGraph(800, 600, MenuButtonImage[selectedMenu], true);

    if (data.Rank >= 1 && data.Rank <= 10) {
        DrawGraph(600, 280, RankUI[data.Rank], true);
    }
}

void Result::ExecuteMenu()
{
    switch (selectedMenu)
    {
    case RESULT_RETRY:
        SceneManager::ChangeScene(new PlayScene());
        break;

    //case RESULT_STAGE_SELECT:
        // SceneManager::ChangeScene(new StageSelect());  // 後で実装
    //    break;

    case RESULT_MENU:
        SceneManager::ChangeScene(new MenuScene());
        break;

    //case RESULT_TITLE:
        // SceneManager::ChangeScene(new TitleScene());  // 後で実装
    //    break;

    default:
        break;
    }
}
