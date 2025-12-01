#include "SceneManager.h"
#include "BaseScene.h"

BaseScene* SceneManager::currentScene = nullptr;
BaseScene* SceneManager::nextScene = nullptr;

void SceneManager::Initialize()
{
	currentScene = nullptr;
	nextScene = nullptr;
}

void SceneManager::Terminate()
{
	if (currentScene != nullptr) {
		currentScene->Terminate();
		delete currentScene;
		currentScene = nullptr;
	}

	if (nextScene != nullptr) {
		delete nextScene;
		nextScene = nullptr;
	}
}

void SceneManager::Update()
{
	//次のシーンがあれば現在シーンとの切り替え
	if (nextScene != nullptr)
	{
		if (currentScene != nullptr) {
			currentScene->Terminate();
			delete currentScene;
		}

		currentScene = nextScene;
		currentScene->Initialize();
		nextScene = nullptr;
	}

	//現在シーンの更新
	if (currentScene != nullptr) {
		currentScene->Update();
	}
}

void SceneManager::ChangeScene(BaseScene* scene)
{
	nextScene = scene;
}
