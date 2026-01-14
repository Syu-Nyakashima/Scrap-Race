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
    MV1SetPosition(GoalModelHandle, VGet(0.0f, 0, 0.0f));
    MV1SetRotationXYZ(GoalModelHandle, VGet(0.0f, 90.0f, 0.0f));
    MV1SetScale(GoalModelHandle, VGet(1.0f, 0.5f, 1.0f));

    checkpoints.clear();

    Checkpoint cp;

    // CP0: スタート地点
    cp.pos = VGet(0.0f, 0.0f, 0.0f);
    cp.width = 50.0f;
    cp.height = 100.0f;
    cp.depth = 200.0f;
    checkpoints.push_back(cp);

    // CP1: 第1コーナー
    cp.pos = VGet(-100.0f, 0.0f, -100.0f);
    cp.width = 200.0f;
    cp.height = 50.0f;
    cp.depth = 30.0f;
    checkpoints.push_back(cp);

    // CP2: 第2コーナー
    cp.pos = VGet(50.0f, 0.0f, -250.0f);
    cp.width = 30.0f;
    cp.height = 50.0f;
    cp.depth = 200.0f;
    checkpoints.push_back(cp);

    // CP3: 第3コーナー
    cp.pos = VGet(100.0f, 0.0f, -70.0f);
    cp.width = 200.0f;
    cp.height = 50.0f;
    cp.depth = 30.0f;
    checkpoints.push_back(cp);

    //printfDx("Checkpoints initialized: %d points\n", (int)checkpoints.size());
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
}

void Stage::Draw()
{
    if (StageModelHandle == -1) return;
    MV1DrawModel(StageModelHandle);

    if (GoalModelHandle == -1) return;
    MV1DrawModel(GoalModelHandle);
}

bool Stage::IsInsideCheckpoint(VECTOR carPos, int currentCheckpoint)
{
    if (checkpoints.empty()) return false;

    // 次のチェックポイントをチェック
    int nextCheckpoint = (currentCheckpoint + 1) % checkpoints.size();
    const Checkpoint& cp = checkpoints[nextCheckpoint];

    // チェックポイント内にいるか判定
    float hw = cp.width / 2.0f;
    float hh = cp.height / 2.0f;
    float hd = cp.depth / 2.0f;

    bool inside =
        carPos.x > cp.pos.x - hw &&
        carPos.x < cp.pos.x + hw &&
        carPos.y > cp.pos.y - hh &&
        carPos.y < cp.pos.y + hh &&
        carPos.z > cp.pos.z - hd &&
        carPos.z < cp.pos.z + hd;

    return inside;
}

VECTOR Stage::GetCheckpointPos(int index) const
{
    if (index < 0 || index >= checkpoints.size())
    {
        return VGet(0.0f, 0.0f, 0.0f);
    }
    return checkpoints[index].pos;
}

void Stage::DrawCheckpoints() const
{
    for (int i = 0; i < checkpoints.size(); i++)
    {
        const Checkpoint& cp = checkpoints[i];

        // CP0は赤、それ以外は青
        int color = (i == 0) ? GetColor(255, 0, 0) : GetColor(0, 150, 255);

        float hw = cp.width / 2.0f;
        float hh = cp.height / 2.0f;
        float hd = cp.depth / 2.0f;

        // 上面
        DrawLine3D(VGet(cp.pos.x - hw, cp.pos.y + hh, cp.pos.z - hd),
            VGet(cp.pos.x + hw, cp.pos.y + hh, cp.pos.z - hd), color);
        DrawLine3D(VGet(cp.pos.x + hw, cp.pos.y + hh, cp.pos.z - hd),
            VGet(cp.pos.x + hw, cp.pos.y + hh, cp.pos.z + hd), color);
        DrawLine3D(VGet(cp.pos.x + hw, cp.pos.y + hh, cp.pos.z + hd),
            VGet(cp.pos.x - hw, cp.pos.y + hh, cp.pos.z + hd), color);
        DrawLine3D(VGet(cp.pos.x - hw, cp.pos.y + hh, cp.pos.z + hd),
            VGet(cp.pos.x - hw, cp.pos.y + hh, cp.pos.z - hd), color);

        // 縦線
        DrawLine3D(VGet(cp.pos.x - hw, cp.pos.y - hh, cp.pos.z - hd),
            VGet(cp.pos.x - hw, cp.pos.y + hh, cp.pos.z - hd), color);
        DrawLine3D(VGet(cp.pos.x + hw, cp.pos.y - hh, cp.pos.z - hd),
            VGet(cp.pos.x + hw, cp.pos.y + hh, cp.pos.z - hd), color);
        DrawLine3D(VGet(cp.pos.x + hw, cp.pos.y - hh, cp.pos.z + hd),
            VGet(cp.pos.x + hw, cp.pos.y + hh, cp.pos.z + hd), color);
        DrawLine3D(VGet(cp.pos.x - hw, cp.pos.y - hh, cp.pos.z + hd),
            VGet(cp.pos.x - hw, cp.pos.y + hh, cp.pos.z + hd), color);
    }
}