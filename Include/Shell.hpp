
#pragma once
#define GLFW_INCLUDE_VULKAN
#include <iostream>
#include <stdexcept>
#include <cstdlib>

#include <GLFW/glfw3.h>



class Shell {
public:
    GLFWwindow* window;
    Shell();

    GLFWwindow * GetWindow();
    void Run();

protected:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    double xpos;
    double ypos;
    double xoffset;
    double yoffset;
    bool isFocused;

    bool FramebufferResize = false;
    
    void InitWindow();
    void MainLoop();

    void Cleanup();

    void createSurface();


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
