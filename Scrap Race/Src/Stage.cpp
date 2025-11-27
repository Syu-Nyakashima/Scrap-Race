#include "Stage.h"

void Stage::Initialize()
{
    // モデルハンドルを初期化
    StageModelHandle = -1;
    CheckColModel = -1;
    GoalModelHandle = -1;

    StageModelHandle = MV1LoadModel("Data/Stage/teststage.mv1");
    if (StageModelHandle == -1) printfDx("ステージモデル読み込み失敗\n");

    MV1SetPosition(StageModelHandle, VGet(0.0f, 0.0f, 0.0f));
    MV1SetScale(StageModelHandle, VGet(1.0f, 1.0f, 1.0f));

    // StageModelHandleと同じものを使う
    CheckColModel = StageModelHandle;

    if (CheckColModel != -1){
        // コリジョン情報の構築
        MV1SetupCollInfo(CheckColModel, -1, 8, 8, 8);
    }
    else {
        printfDx("コリジョンモデル読み込み失敗\n");
    }

    // ゴールモデル
    GoalModelHandle = MV1LoadModel("Data/Stage/goal.mv1");
    if (GoalModelHandle == -1) printfDx("ゴールモデル読み込み失敗\n");
    MV1SetPosition(GoalModelHandle, VGet(120.0f, 0, -50.0f));
    MV1SetScale(GoalModelHandle, VGet(1.0f, 0.5f, 1.0f));

    GoalPos = VGet(120.0f, 0, -50.0f);
    GoalWidth = 100.0f;
    GoalHeight = 50.0f;
    GoalDepth = 10.0f;
    isGoal = false;
}

void Stage::Terminate()
{
    if (CheckColModel != -1)
    {
        MV1TerminateCollInfo(CheckColModel, -1);
    }

    if (StageModelHandle != -1) {
        MV1DeleteModel(StageModelHandle);
        StageModelHandle = -1;
    }

    if (GoalModelHandle != -1) {
        MV1DeleteModel(GoalModelHandle);
        GoalModelHandle = -1;
    }
}

void Stage::Update()
{
    
    // ===== 地面を描画 =====
    /*
    VECTOR p1 = VGet(-10.0f, 0.0f, -10.0f);
    VECTOR p2 = VGet(10.0f, 0.0f, -10.0f);
    VECTOR p3 = VGet(-10.0f, 0.0f, 10.0f);
    VECTOR p4 = VGet(10.0f, 0.0f, 10.0f);
    int groundColor = GetColor(100, 200, 100);
    DrawTriangle3D(p1, p2, p3, groundColor, TRUE);
    DrawTriangle3D(p3, p2, p4, groundColor, TRUE);
    
    // ===== 左の壁 =====
    VECTOR lw1 = VGet(-50.0f, 0.0f, -200.0f);
    VECTOR lw2 = VGet(-50.0f, 3.0f, -200.0f);
    VECTOR lw3 = VGet(-50.0f, 0.0f, 200.0f);
    VECTOR lw4 = VGet(-50.0f, 3.0f, 200.0f);
    int wallColor = GetColor(150, 150, 150);
    DrawTriangle3D(lw1, lw2, lw3, wallColor, TRUE);
    DrawTriangle3D(lw3, lw2, lw4, wallColor, TRUE);

    // ===== 右の壁 =====
    VECTOR rw1 = VGet(50.0f, 0.0f, -200.0f);
    VECTOR rw2 = VGet(50.0f, 3.0f, -200.0f);
    VECTOR rw3 = VGet(50.0f, 0.0f, 200.0f);
    VECTOR rw4 = VGet(50.0f, 3.0f, 200.0f);
    DrawTriangle3D(rw1, rw3, rw2, wallColor, TRUE);
    DrawTriangle3D(rw3, rw4, rw2, wallColor, TRUE);

    for (float z = -200; z < 200; z+=10.0f) {
        DrawCone3D(VGet(51.0f, 10.0f, z), VGet(51.0f, 0.0f, z), 3.0f, 16, GetColor(0, 0, 255), GetColor(255, 255, 255), TRUE);
        DrawCone3D(VGet(-51.0f, 10.0f, z), VGet(-51.0f, 0.0f, z), 3.0f, 16, GetColor(0, 0, 255), GetColor(255, 255, 255), TRUE);
    }
    */

    //ゴール判定(デバッグ用)
    //上面
    DrawLine3D(VGet(GoalPos.x - GoalWidth / 2, 25.0f, GoalPos.z - GoalDepth / 2),
        VGet(GoalPos.x + GoalWidth / 2, 25.0f, GoalPos.z - GoalDepth / 2),
        GetColor(255, 255, 0));
    DrawLine3D(VGet(GoalPos.x + GoalWidth / 2, 25.0f, GoalPos.z - GoalDepth / 2),
        VGet(GoalPos.x + GoalWidth / 2, 25.0f, GoalPos.z + GoalDepth / 2),
        GetColor(255, 255, 0));
    DrawLine3D(VGet(GoalPos.x + GoalWidth / 2, 25.0f, GoalPos.z + GoalDepth / 2),
        VGet(GoalPos.x - GoalWidth / 2, 25.0f, GoalPos.z + GoalDepth / 2),
        GetColor(255, 255, 0));
    DrawLine3D(VGet(GoalPos.x - GoalWidth / 2, 25.0f, GoalPos.z + GoalDepth / 2),
        VGet(GoalPos.x - GoalWidth / 2, 25.0f, GoalPos.z - GoalDepth / 2),
        GetColor(255, 255, 0));

    DrawLine3D(VGet(GoalPos.x - GoalWidth / 2, 0.0f, GoalPos.z - GoalDepth / 2),
        VGet(GoalPos.x - GoalWidth / 2, 25.0f, GoalPos.z - GoalDepth / 2),
        GetColor(255, 255, 0));
    DrawLine3D(VGet(GoalPos.x + GoalWidth / 2, 0.0f, GoalPos.z - GoalDepth / 2),
        VGet(GoalPos.x + GoalWidth / 2, 25.0f, GoalPos.z - GoalDepth / 2),
        GetColor(255, 255, 0));
    DrawLine3D(VGet(GoalPos.x + GoalWidth / 2, 0.0f, GoalPos.z + GoalDepth / 2),
        VGet(GoalPos.x + GoalWidth / 2, 25.0f, GoalPos.z + GoalDepth / 2),
        GetColor(255, 255, 0));
    DrawLine3D(VGet(GoalPos.x - GoalWidth / 2, 0.0f, GoalPos.z + GoalDepth / 2),
        VGet(GoalPos.x - GoalWidth / 2, 25.0f, GoalPos.z + GoalDepth / 2),
        GetColor(255, 255, 0));
}

void Stage::Draw()
{
    if (StageModelHandle == -1) return;
    MV1DrawModel(StageModelHandle);

    if (GoalModelHandle == -1) return;
    MV1DrawModel(GoalModelHandle);
}

bool Stage::CheckGoal(VECTOR playerPos)
{
    if (isGoal) return true;
    
    static bool wasInside = false;
    static float inZ = 0.0f;
    static float outZ = 0.0f;

    float hw = GoalWidth / 2.0f;
    float hh = GoalHeight / 2.0f;
    float hd = GoalDepth / 2.0f;

    //ゴール判定の内側にいるか
    bool nowInside =
        playerPos.x > GoalPos.x - hw &&
        playerPos.x < GoalPos.x + hw &&
        playerPos.y > GoalPos.y - hh &&
        playerPos.y < GoalPos.y + hh &&
        playerPos.z > GoalPos.z - hd &&
        playerPos.z < GoalPos.z + hd;

    if (!wasInside && nowInside) {
        inZ = playerPos.z;
    }

    if (wasInside && !nowInside) {
        outZ = playerPos.z;

        if (inZ < outZ) {
            isGoal = true;
            return true;
        }
    }

    wasInside = nowInside;
    return false;
}
