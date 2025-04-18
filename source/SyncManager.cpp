#include "SyncManager.hpp"


SyncManager::SyncManager(const VkDevice& device) : m_device(device) {}

SyncManager::~SyncManager() {
    cleanup();
}

void SyncManager::Initialize(uint32_t maxFramesInFlight){
    m_frames.resize(maxFramesInFlight);
    for (auto& frame : m_frames) {
        frame.imageAvailable = CreateSemaphore();
        frame.renderFinished = CreateSemaphore();
        frame.inFlight = createFenceInternal(true); // Initially signaled
    }
}


VkSemaphore SyncManager::CreateSemaphore() {
    VkSemaphore semaphore;
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &semaphore) != VK_SUCCESS) {
        throw std::runtime_error("failed to create semaphore!");
    }

    return semaphore;
}

VkFence SyncManager::CreateFence() {
    VkFence fence;
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    if (vkCreateFence(m_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }

    return fence;
}


void SyncManager::cleanup() {
    for (auto& frame : m_frames) {
        vkDestroySemaphore(m_device, frame.imageAvailable, nullptr);
        vkDestroySemaphore(m_device, frame.renderFinished, nullptr);
        vkDestroyFence(m_device, frame.inFlight, nullptr);
    }
    m_frames.clear();
}