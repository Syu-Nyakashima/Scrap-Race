#include "BaseScene.h"
#include "DxLib.h"

void BaseScene::UpdateKeyState()
{
    // 前フレームの状態を保持
    prevUpKey = nowUpKey;
    prevDownKey = nowDownKey;
	prevLeftKey = nowLeftKey;
	prevRightKey = nowRightKey;
    prevEnterKey = nowEnterKey;
    prevSpaceKey = nowSpaceKey;

    // 今フレームの状態を取得
    nowUpKey = (CheckHitKey(KEY_INPUT_UP) == 1 || CheckHitKey(KEY_INPUT_W) == 1);
    nowDownKey = (CheckHitKey(KEY_INPUT_DOWN) == 1 || CheckHitKey(KEY_INPUT_S) == 1);
	nowLeftKey = (CheckHitKey(KEY_INPUT_LEFT) == 1 || CheckHitKey(KEY_INPUT_A) == 1);
	nowRightKey = (CheckHitKey(KEY_INPUT_RIGHT) == 1 || CheckHitKey(KEY_INPUT_D) == 1);
    nowEnterKey = (CheckHitKey(KEY_INPUT_RETURN) == 1);
    nowSpaceKey = (CheckHitKey(KEY_INPUT_SPACE) == 1);

}

bool BaseScene::IsKeyJustPressed(int keyCode) const
{
    switch (keyCode)
    {
    case KEY_INPUT_UP:
    case KEY_INPUT_W:
        return !prevUpKey && nowUpKey;

    case KEY_INPUT_DOWN:
    case KEY_INPUT_S:
        return !prevDownKey && nowDownKey;

	case KEY_INPUT_LEFT:
	case KEY_INPUT_A:
		return !prevLeftKey && nowLeftKey;

	case KEY_INPUT_RIGHT:
	case KEY_INPUT_D:
		return !prevRightKey && nowRightKey;

    case KEY_INPUT_RETURN:
        return !prevEnterKey && nowEnterKey;

    case KEY_INPUT_SPACE:
        return !prevSpaceKey && nowSpaceKey;

    default:
        return false;
    
    }
}
