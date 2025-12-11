#pragma once

class BaseScene {
public:
    virtual ~BaseScene() {}

    virtual void Initialize() = 0;  // 純粋仮想関数
    virtual void Terminate() = 0;
    virtual void Update() = 0;

protected:
    // キー入力管理
    void UpdateKeyState();
    bool IsKeyJustPressed(int keyCode) const;

private:
    // 現在のキー状態
    bool nowUpKey;
    bool nowDownKey;
    bool nowEnterKey;
    bool nowSpaceKey;

    // 前フレームのキー状態
    bool prevUpKey = false;
    bool prevDownKey = false;
    bool prevEnterKey = false;
    bool prevSpaceKey = false;

};