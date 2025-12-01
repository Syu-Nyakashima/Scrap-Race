#pragma once
#include "DxLib.h"

enum ResultType
{
    RESULT_GAMEOVER,
    RESULT_CLEAR
};

enum ResultMenuSelect
{
    MENU_RETRY,         // リトライ(同じステージ)
    MENU_STAGE_SELECT,  // ステージ選択
    MENU_TITLE,         // タイトルに戻る
    MENU_MAX
};

// リザルト情報を渡すための構造体
struct ResultData
{
    float raceTime;           // 走行タイム
    float finalSpeed;         // 最終速度
    float finalAcceleration;  // 最終加速力
    // 他のステータスも追加
    int crashCount;           // クラッシュ回数
};

class Result
{
private:
    ResultType type;
    ResultData data;
    int selectedMenu;  // 選択中のメニュー

public:
    Result(ResultType resultType, const ResultData& resultData);
    ~Result();
    void Initialize();
    void Terminate();
    void Update();
    void Draw();

private:
    void UpdateMenu();  // メニュー選択処理
    void DrawGameOver();
    void DrawClear();
};
