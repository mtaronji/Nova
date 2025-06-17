
#pragma once

#define GLFW_INCLUDE_VULKAN
#include "IRenderLoopClient.hpp"

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <GLFW/glfw3.h>
#include <chrono>
#include <thread>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class IRenderLoopClient;

class Shell {
public:
    GLFWwindow* window;
   
    Shell();

    GLFWwindow * GetWindow()const {return window;};

    void Run(IRenderLoopClient *app);
    
    float GetX() { return static_cast<float>(xpos); }
    float GetY() { return static_cast<float>(ypos); }
    float GetDeltaX() { 
        auto out = static_cast<float>(deltaX); 
        deltaX = 0.0;
        return out;
    }
    float GetDeltaY() {
        auto out = static_cast<float>(deltaY);
        deltaY = 0.0;
        return out;
    }


protected:
    virtual uint32_t GetWidth() const { return 800; }
    virtual uint32_t GetHeight() const { return 600; }

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    double xpos;
    double ypos;
    double lastX;
    double lastY;
    double xoffset;
    double yoffset;
    double deltaX = 0.0f;
    double deltaY = 0.0f;
    bool isFocused;
    bool FramebufferResize = false;
    bool firstMouse = true;
 
    TimePoint previousTime;

    void InitWindow();
    void MainLoop(IRenderLoopClient *app);

    void Cleanup();

   // === CALLBACK FUNCTION DECLARATIONS ===

    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    virtual void OnFramebufferResize(GLFWwindow * window, int width, int height);

    static void KeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    virtual void OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods);

    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    virtual void OnMouseButton(GLFWwindow* window, int button, int action, int mods);

    static void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
    virtual void OnCursorPosition(GLFWwindow* window, double xpos, double ypos);

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    virtual void OnScroll(GLFWwindow* window, double xoffset, double yoffset);

    static void WindowFocusCallback(GLFWwindow* window, int focused);
    virtual void OnWindowFocus(GLFWwindow* window, int focused);

    static void DropCallback(GLFWwindow* window, int count, const char** paths);
    virtual void OnDrop(GLFWwindow* window, int count, const char** paths);

    static void JoystickCallback(int jid, int event);
    virtual void OnJoystick(int jid, int event);

    // === SETUP FUNCTION DECLARATION ===

    virtual void InitCallbacks(GLFWwindow* window);
 

};
