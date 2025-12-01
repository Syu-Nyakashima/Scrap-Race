#pragma once

class BaseScene;

class SceneManager {
public:
	static void Initialize();
	static void Terminate();
	static void Update();

	// シーン変更
	static void ChangeScene(BaseScene* scene);
private:
	SceneManager() {}  // インスタンス化禁止

	static BaseScene* currentScene;  // 現在のシーン
	static BaseScene* nextScene;     // 次のシーン
};