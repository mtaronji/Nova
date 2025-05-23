#include "GPU.hpp"
#include "VulkanEngine.hpp"

GPU::GPU(std::shared_ptr<VulkanEngine> engine) : engine(engine){
    auto instance = engine->GetInstance();
    PickPhysicalDevice(instance);
    CreateLogicalDevice();
    QueryAll();
}

void GPU::Cleanup() {
    if (device) {
        vkDestroyDevice(device, nullptr);
        device = VK_NULL_HANDLE;
    }
}

void GPU::PickPhysicalDevice(VkInstance instance) {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("Failed to find GPUs with Vulkan support");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (const auto& dev : devices) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(dev, &props);
        std::cout << "Found GPU: " << props.deviceName << std::endl;

        // Prepare the extended feature structs
        VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures separateLayoutsFeature{};
        separateLayoutsFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES;

        VkPhysicalDeviceFeatures2 features2{};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &separateLayoutsFeature;

        // Query supported features with the extended chain
        vkGetPhysicalDeviceFeatures2(dev, &features2);

        // Check if the separate layout feature is supported
        if (separateLayoutsFeature.separateDepthStencilLayouts == VK_TRUE) {
            std::cout << "  -> separateDepthStencilLayouts supported" << std::endl;
            physicalDevice = dev;
        
            break;  // or choose best candidate
        } else {
            std::cout << "  -> separateDepthStencilLayouts NOT supported" << std::endl;
        }
       
                 
    }
  
    if (physicalDevice == VK_NULL_HANDLE)
        throw std::runtime_error("Failed to select a suitable GPU");

    // Find graphics queue family
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    for (uint32_t i = 0; i < queueFamilies.size(); ++i) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
            break;
        }
    }
 
}

void GPU::CreateLogicalDevice() {

    QueueFamilyIndices indices = FindQueueFamilies();
    auto deviceExtensions = engine->GetDeviceExtensions();
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    //advanced stencil layouts
    // VkPhysicalDeviceSeparateDepthStencilLayoutsFeatures separateDSLayoutsEnable{};
    // separateDSLayoutsEnable.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SEPARATE_DEPTH_STENCIL_LAYOUTS_FEATURES;
    // separateDSLayoutsEnable.separateDepthStencilLayouts = VK_TRUE;
    
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;
    

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    //createInfo.pNext = &separateDSLayoutsEnable;
    createInfo.pNext = nullptr;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

QueueFamilyIndices GPU::FindQueueFamilies() {
    auto surface = engine->GetSurface();
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

void GPU::QueryAll() {
    vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
    vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, nullptr);
    queueFamilies.resize(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueFamilies.data());

    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
    extensionProperties.resize(extensionCount);
    vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensionProperties.data());
}

VkPhysicalDeviceProperties& GPU::GetDeviceProperties() {
    return deviceProperties;
}

VkPhysicalDeviceFeatures& GPU::GetDeviceFeatures()  {
    return deviceFeatures;
}

VkPhysicalDeviceMemoryProperties& GPU::GetMemoryProperties()  {
    return memoryProperties;
}

std::vector<VkQueueFamilyProperties>& GPU::GetQueueFamilies()  {
    return queueFamilies;
}

std::vector<VkExtensionProperties>& GPU::GetDeviceExtensions()  {
    return extensionProperties;
}

bool GPU::IsFormatSupported(VkFormat format, VkImageTiling tiling, VkFormatFeatureFlags features) {
    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
        return true;
    }
    if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
        return true;
    }
    return false;
}

std::vector<VkFormat> GPU::GetSupportedFormats(VkImageTiling tiling, VkFormatFeatureFlags features) {
    std::vector<VkFormat> supported;

    for (int format = VK_FORMAT_UNDEFINED + 1; format <= VK_FORMAT_ASTC_12x12_SRGB_BLOCK; ++format) {
        if (IsFormatSupported(static_cast<VkFormat>(format), tiling, features)) {
            supported.push_back(static_cast<VkFormat>(format));
        }
    }

    return supported;
}

uint32_t GPU::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)  {
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; ++i) {
        if ((typeFilter & (1 << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}