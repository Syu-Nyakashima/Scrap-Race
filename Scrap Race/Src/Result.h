#pragma once
#include "BaseScene.h"
#include "EnemyCPU.h"

enum ResultType
{
    RESULT_GAMEOVER,
    RESULT_CLEAR
};

enum ResultSelect
{
    RESULT_RETRY,         // リトライ(同じステージ)
    //RESULT_STAGE_SELECT,  //ステージセレクト
    RESULT_MENU,          // メニュー選択
    //RESULT_TITLE,         // タイトルに戻る
    RESULT_MAX
};

// リザルト情報を渡すための構造体
struct ResultData
{
	AIDifficulty enemyDifficulty; // 敵の難易度
    float raceTime;           // 走行タイム
    float finalSpeed;         // 最終速度
    float finalHp;
	int Rank;                // 最終順位
};

class Result : public BaseScene
{
public:
    Result(ResultType resultType, const ResultData& resultData);
    ~Result();
    void Initialize() override;
    void Terminate() override;
    void Update() override;

private:
    void Draw();            // 描画処理
    void UpdateMenu();      // メニュー選択更新
    void DrawGameOver();    // ゲームオーバー画面
    void DrawClear();       // クリア画面
    void ExecuteMenu();     // メニュー実行
private:
    ResultType type;
    ResultData data;
	int ResultImage; // リザルト背景画像
	int RankUI[10];  // 順位表示画像
	int MenuButtonImage[2]; // メニュー背景画像
	int RetryButtonImage[2]; // リトライボタン画像

    int selectedMenu;  // 選択中のメニュー

    int displayTimer; // 表示用の時間
};
