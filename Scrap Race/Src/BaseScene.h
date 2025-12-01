#pragma once

class BaseScene {
public:
    virtual ~BaseScene() {}

    virtual void Initialize() = 0;  // ƒˆ‰¼‘zŠÖ”
    virtual void Terminate() = 0;
    virtual void Update() = 0;
};