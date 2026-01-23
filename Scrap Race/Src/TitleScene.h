#pragma once
#include "BaseScene.h"

class TitleScene : public BaseScene {
public:
	TitleScene();
	~TitleScene();

	void Initialize() override;
	void Terminate() override;
	void Update() override;

private:
	void Draw();			// •`‰æˆ—

	int TitleImage;		  // ƒ^ƒCƒgƒ‹‰æ–Ê‚Ì”wŒi
};