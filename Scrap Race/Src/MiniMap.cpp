#include "MiniMap.h"
#include "CarBase.h"
#include "Stage.h"
#include <algorithm>

MiniMap::MiniMap(int x, int y, float w, float h, float worldW, float worldH, Stage* stageRef)
{
	mapX = x;
	mapY = y;
	mapWidth = w;
	mapHeight = h;
	worldWidth = worldW;
	worldHeight = worldH;
	scale = (std::min)(mapWidth / worldWidth, mapHeight / worldHeight);

	stage = stageRef;
}

MiniMap::~MiniMap()
{
}

void MiniMap::Initialize()
{
    CreateStageTopViewImage();

    mapScreen = MakeScreen(mapWidth, mapHeight, TRUE);

    if (mapScreen == -1)
    {
        printfDx("MiniMap: Failed to create screen buffer\n");
        return;
    }

    SetDrawScreen(mapScreen);
    ClearDrawScreen();

    if (stageTopViewImageHandle != -1)
    {
        //DrawExtendGraph(0, 0, mapWidth, mapHeight, stageTopViewImageHandle, TRUE);
    }
    else
    {
        DrawBox(0, 0, mapWidth, mapHeight, GetColor(50, 50, 50), TRUE);
    }

    // ステージレイアウトを描画
    if (stage != nullptr)
    {
        DrawStageLayout();
    }

	SetDrawScreen(DX_SCREEN_BACK);

    bgColor = GetColor(50, 50, 50);  // 灰色
    frameColor = GetColor(255, 255, 255);
    playerColor = GetColor(0, 150, 255);
    enemyColor = GetColor(255, 50, 50);
    enemyDeadColor = GetColor(100, 100, 100);
    checkpointColor = GetColor(255, 255, 0);
}

void MiniMap::Terminate()
{
}

void MiniMap::Draw(const std::vector<CarBase*>& allCars, int Player)
{
    // 1. 事前描画したスクリーンバッファを半透明で描画
    SetDrawBlendMode(DX_BLENDMODE_ALPHA, 200);
    DrawGraph(mapX, mapY, mapScreen, TRUE);
    SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

    // 2. 枠線を描画
    DrawBox(mapX, mapY, mapX + mapWidth, mapY + mapHeight, frameColor, FALSE);

    // 3. タイトルを描画（デフォルトフォント使用）
    SetFontSize(12);
    DrawString(mapX + 5, mapY + 5, "MAP", GetColor(200, 200, 200));
    SetFontSize(16);  // 元に戻す

    // 4. 全車両を描画
    for (int i = 0; i < allCars.size(); i++)
    {
        if (allCars[i] == nullptr) continue;

        VECTOR pos = allCars[i]->GetPosition();
        int miniX, miniY;
        WorldToMiniMap(pos.x, pos.z, miniX, miniY);

        // ミニマップ座標をスクリーン座標に変換
        int screenX = mapX + miniX;
        int screenY = mapY + miniY;

        // ミニマップの範囲内かチェック
        if (screenX < mapX || screenX > mapX + mapWidth ||
            screenY < mapY || screenY > mapY + mapHeight)
        {
            continue;  // 範囲外は描画しない
        }

        if (i == 0)
        {
            // プレイヤー（青い三角形で方向も表示）
            float angle = allCars[i]->angle;
            float rad = -angle * DX_PI_F / 180.0f;

            // 三角形のサイズ
            float triSize = 6.0f;

            // 三角形の3つの頂点を計算
            int x1 = screenX + (int)(sin(rad) * triSize);
            int y1 = screenY - (int)(cos(rad) * triSize);

            int x2 = screenX + (int)(sin(rad + 2.4f) * triSize * 0.6f);
            int y2 = screenY - (int)(cos(rad + 2.4f) * triSize * 0.6f);

            int x3 = screenX + (int)(sin(rad - 2.4f) * triSize * 0.6f);
            int y3 = screenY - (int)(cos(rad - 2.4f) * triSize * 0.6f);

            // 三角形を描画
            DrawTriangle(x1, y1, x2, y2, x3, y3, playerColor, TRUE);
            DrawTriangle(x1, y1, x2, y2, x3, y3, frameColor, FALSE);
        }
        else
        {
            // 敵
            if (allCars[i]->IsAlive())
            {
                // 生存している敵（赤い円）
                DrawCircle(screenX, screenY, 4, enemyColor, TRUE);
                DrawCircle(screenX, screenY, 4, frameColor, FALSE);
            }
            else
            {
                // 死亡した敵（グレーの小さい円）
                DrawCircle(screenX, screenY, 2, enemyDeadColor, TRUE);
            }
        }
    }

}

void MiniMap::WorldToMiniMap(float worldX, float worldZ, int& miniX, int& miniY)
{
    float offsetX = worldWidth / 2.0f;
    float offsetZ = worldHeight / 2.0f;

    // 通常の座標変換
    int tempX = mapWidth / 2 + (int)((worldX + offsetX) * scale) - (int)(worldWidth * scale / 2);
    int tempY = mapHeight / 2 + (int)((worldZ + offsetZ) * scale) - (int)(worldHeight * scale / 2);

    // 180度回転（中心を軸に反転）
    miniX =tempX;
    miniY =tempY;
}

void MiniMap::CreateStageTopViewImage()
{
    if (stage == nullptr) {
        return;
    }

    int topViewWidth = 512;
    int topViewHeight = 512;
    int topViewScreen = MakeScreen(topViewWidth, topViewHeight, TRUE);

    if (topViewScreen == -1)
    {
        return;
    }

    int oldScreen = GetDrawScreen();

    SetDrawScreen(topViewScreen);
    ClearDrawScreen();

    // 真上から見下ろすカメラを設定
    VECTOR cameraPos = VGet(0.0f, 200.0f, 0.0f);
    VECTOR cameraTarget = VGet(0.0f, 0.0f, 0.0f);
    VECTOR cameraUp = VGet(0.0f, 0.0f, -1.0f);

    SetCameraPositionAndTargetAndUpVec(cameraPos, cameraTarget, cameraUp);

    // 平行投影で描画
    SetupCamera_Ortho(worldWidth * 1.2f);

    // ステージを描画
    stage->Draw();

    // スクリーンの内容を画像として保存
    stageTopViewImageHandle = MakeGraph(topViewWidth, topViewHeight);
    GetDrawScreenGraph(0, 0, topViewWidth, topViewHeight, stageTopViewImageHandle, topViewScreen);

    // 描画先を元に戻す
    SetDrawScreen(oldScreen);

    // 一時スクリーンを削除
    DeleteGraph(topViewScreen);
}

void MiniMap::DrawStageLayout()
{
    if (stage == nullptr) return;

    int totalCP = stage->GetTotalCheckpoints();

    if (totalCP == 0) return;

    // チェックポイントを描画
    for (int i = 0; i < totalCP; i++)
    {
        VECTOR cpPos = stage->GetCheckpointPos(i);
        int miniX, miniY;
        WorldToMiniMap(cpPos.x, cpPos.z, miniX, miniY);

        // チェックポイントを黄色い円で描画
        DrawCircle(miniX, miniY, 4, checkpointColor, FALSE);

        // CP番号を表示
        SetFontSize(10);
        DrawFormatString(miniX + 6, miniY - 5, checkpointColor, "%d", i);
        SetFontSize(16);
    }

    // コースラインを描画（チェックポイントを線で結ぶ）
    for (int i = 0; i < totalCP; i++)
    {
        VECTOR cp1 = stage->GetCheckpointPos(i);
        VECTOR cp2 = stage->GetCheckpointPos((i + 1) % totalCP);

        int x1, y1, x2, y2;
        WorldToMiniMap(cp1.x, cp1.z, x1, y1);
        WorldToMiniMap(cp2.x, cp2.z, x2, y2);

        // コースラインを緑色で描画
        DrawLine(x1, y1, x2, y2, GetColor(100, 200, 100), 2);
    }
}