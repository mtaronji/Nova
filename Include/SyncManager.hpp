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
    
    FrameSync& GetFrame(uint32_t index) {
        return m_frames[index];
    }

    // Cleanup all semaphores
    void Cleanup();

protected:
    VkDevice m_device;
    std::vector<FrameSync> m_frames;

    // Helper function to create a semaphore
    VkSemaphore CreateSemaphore();
    VkFence CreateFence(bool signaled);
};