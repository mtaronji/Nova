#include "SyncManager.hpp"


SyncManager::SyncManager(VkDevice device ) : m_device(device) {}

SyncManager::~SyncManager() {
    cleanup();
}

void SyncManager::Initialize(uint32_t maxFramesInFlight){
    m_frames.resize(maxFramesInFlight);
    for (int i = 0; i < m_frames.size(); i++) {
        m_frames[i].imageAvailable = CreateSemaphore();
        m_frames[i].renderFinished = CreateSemaphore();
        m_frames[i].inFlight = i == 0 ? CreateFence(true) : CreateFence(false); 
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

VkFence SyncManager::CreateFence(bool signaled) {
    VkFence fence;
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if(signaled) {fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;}

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