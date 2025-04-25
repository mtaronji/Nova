#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <memory>

struct FrameSync {
    VkSemaphore imageAvailable;
    VkSemaphore renderFinished;
    VkFence inFlight;
};

class SyncManager {
public:
    SyncManager(VkDevice device);
    ~SyncManager();
    void Initialize(uint32_t maxFramesInFlight);
    // Create a semaphore for synchronization
    VkSemaphore CreateSemaphore();
    VkFence CreateFence();
    
    FrameSync& GetFrame(uint32_t index) {
        return m_frames[index];
    }

    // Cleanup all semaphores
    void cleanup();

protected:
    VkDevice m_device;
    std::vector<FrameSync> m_frames;

    // Helper function to create a semaphore
    VkSemaphore createSemaphoreInternal();

    VkFence createFenceInternal(bool signaled);
};