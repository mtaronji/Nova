
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
#include <functional>
#include <memory>
#include <Observable.hpp>
#include "MouseEvents.hpp"
#include "KeyboardEvents.hpp"


class IRenderLoopClient;

class Shell {

public:
    GLFWwindow* window;
   
    Shell();

    GLFWwindow * GetWindow()const {return window;};
    void Run(IRenderLoopClient *app);

    //we only want to expose the Observable part 
    std::shared_ptr<Observable<MouseButtonEvent>> MouseButtons() { 
        return _mouseButtons;
    }
    std::shared_ptr<Observable<KeyPressEvent>> Keys() {
        return _keys;
    }
    std::shared_ptr<Observable<MouseMoveEvent>> MouseLocation() {
        return _mouseLocation;
    }


protected:
    virtual uint32_t GetWidth() const { return 800; }
    virtual uint32_t GetHeight() const { return 600; }

    using Clock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<Clock>;

    std::shared_ptr<Signal<MouseButtonEvent>> _mouseButtons;
    std::shared_ptr<Signal<KeyPressEvent>>  _keys;
    std::shared_ptr<Signal<MouseMoveEvent>> _mouseLocation;

    bool FramebufferResize = false;
    bool isFocused = false;
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
