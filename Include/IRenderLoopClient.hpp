#pragma once

class IRenderLoopClient {
    public:
    virtual void Start() = 0;
    virtual void Init() = 0;
    virtual void Update(float deltaTime = 0.0f) = 0; // Frame logic (deltaTime optional)
    virtual void Render() = 0;             // Record & submit commands

    virtual void Shutdown() = 0;

    virtual ~IRenderLoopClient() = default;
};