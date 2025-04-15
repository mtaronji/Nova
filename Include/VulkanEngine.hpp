#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <string.h>
#include <vector>
#include<stdlib.h>
#include <stdexcept>

// Vulkan Engine


class VulkanEngine{
    public:
        #ifdef NDEBUG
        const bool EnableValidationLayers = false;
        #else
        const bool EnableValidationLayers = true;
        #endif
        VkInstance instance;
        VkDebugUtilsMessengerEXT debugMessenger;
        VkSurfaceKHR surface;

        //the way memory is allocated in vulkan is that you first:
        //create a vulkan buffer
        //allocate device memory for it 
        //bind the buffer
        //VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT mean you can use the buffer as a src of a transfer or the destination of one respectfully
        
        VulkanEngine(GLFWwindow* window);

        VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) ;

        void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    
        void SetupDebugMessenger();
    
        void CreateSurface(GLFWwindow *window);
        GLFWwindow * GetWindow();

        private:
            const std::vector<const char*> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
            };
            
            const std::vector<const char*> deviceExtensions = {
                VK_KHR_SWAPCHAIN_EXTENSION_NAME
            };

            GLFWwindow *window;
            void CreateInstance();
            bool CheckValidationLayerSupport();
            std::vector<const char*> GetRequiredExtensions();

            static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};