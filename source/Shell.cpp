#include "Shell.hpp"


Shell::Shell(){
    InitWindow();
 
}

void Shell::InitWindow() {

    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    this->window = glfwCreateWindow(GetWidth(), GetHeight(), "Shell", nullptr, nullptr);
    if(!window){ throw std::runtime_error("Failed to create Window");}

    glfwSetWindowUserPointer(window, this);
    this->InitCallbacks(this->window);
}

void Shell::MainLoop(IRenderLoopClient* app) {
    const float fixedDeltaTime = 1.0f / 60.0f;
    const float targetFrameTime = 1.0f / 144.0f; // 144 FPS cap
    TimePoint previousTime = Clock::now();
    float accumulator = 0.0f;

    while (!glfwWindowShouldClose(this->window)) {
        glfwPollEvents();

        TimePoint currentTime = Clock::now();
        std::chrono::duration<float> elapsed = currentTime - previousTime;
        float deltaTime = elapsed.count();

        app->Update(fixedDeltaTime); 
        app->Render();

        // Print FPS based on full frame time (including update+render+sleep)
        std::cout << "FPS : " << 1.0f / deltaTime << std::endl;

        // Sleep if frame is faster than target
        if (deltaTime < targetFrameTime) {
            std::this_thread::sleep_for(std::chrono::duration<float>(targetFrameTime - deltaTime));
        }

        // Update previousTime to after sleep to measure full frame time next iteration
        previousTime = Clock::now();
    }
}


void Shell::Run(IRenderLoopClient* app) {
    MainLoop(app);
    Cleanup();
}

void Shell::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {

    // Get the instance of Shell (or derived class) from the user pointer
    Shell* shell = static_cast<Shell*>(glfwGetWindowUserPointer(window));
    if (shell) {
        shell->OnFramebufferResize(window, width, height); // Call the virtual function
    }
    else{
        throw std::invalid_argument("Unable to retreive the Framebuffer resize callback");
    }
}
void Shell::OnFramebufferResize(GLFWwindow* window, int width, int height) {
 
    this->FramebufferResize = true;
}

void Shell::KeyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Shell* shell = static_cast<Shell*>(glfwGetWindowUserPointer(window));
    if (shell) {
        shell->OnKeyPress(window, key, scancode, action,mods); // Call the virtual function
    }
    else{
        throw std::invalid_argument("Unable to retreive the KeyPress callback");
    }
}
void Shell::OnKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Shell::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {

    Shell* shell = static_cast<Shell*>(glfwGetWindowUserPointer(window));
    if (shell) {
        shell->OnMouseButton(window, button, action, mods); // Call the virtual function
    }
    else{
        throw std::invalid_argument("Unable to retreive the Mouse Button Callback");
    }
}
void  Shell::OnMouseButton(GLFWwindow*, int, int, int){

}

void Shell::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {

  
    Shell* shell = static_cast<Shell*>(glfwGetWindowUserPointer(window));
    if (shell) {
        shell->OnCursorPosition(window, xpos, ypos); // Call the virtual function
    }
    else{
        throw std::invalid_argument("Unable to retreive the Mouse Button Callback");
    }
}
void Shell::OnCursorPosition(GLFWwindow*, double xpos, double ypos){
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        this->xpos = xpos;
        this->ypos = ypos;
        firstMouse = false;
        return;
    }
    this->lastX = this->xpos;
    this->lastY = this->ypos;
    this->xpos = xpos;
    this->ypos = ypos;
    this->deltaX = this->xpos - this->lastX;
    this->deltaY = this->ypos - this->lastY;

}

void Shell::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {

    Shell* shell = static_cast<Shell*>(glfwGetWindowUserPointer(window));
    if (shell) {
        shell->OnScroll(window, xoffset, yoffset); // Call the virtual function
    }
    else{
        throw std::invalid_argument("Unable to retreive the Mouse Button Callback");
    }
}
void Shell::OnScroll(GLFWwindow*, double xoffset, double yoffset){
    this->xoffset =xoffset;
    this->yoffset = yoffset;
}

void Shell::WindowFocusCallback(GLFWwindow* window, int focused) {
    Shell* shell = static_cast<Shell*>(glfwGetWindowUserPointer(window));
    if (shell) {
        shell->OnWindowFocus(window, focused); // Call the virtual function
    }
    else{
        throw std::invalid_argument("Unable to retreive the Mouse Button Callback");
    }
}
void Shell::OnWindowFocus(GLFWwindow* window, int focused){
    if(focused == 1){
        this->isFocused = true;
    }
    else{
        this->isFocused = false;
    }
}

void Shell::DropCallback(GLFWwindow* window, int count, const char** paths) {

    Shell* shell = static_cast<Shell*>(glfwGetWindowUserPointer(window));
    if (shell) {
        shell->OnDrop(window, count, paths); // Call the virtual function
    }
    else{
        throw std::invalid_argument("Unable to retreive the Mouse Button Callback");
    }
}
void Shell::OnDrop(GLFWwindow*, int count, char const** paths){

}
void Shell::JoystickCallback(int jid, int event) {
    if (event == GLFW_CONNECTED)
        std::cout << "Joystick " << jid << " connected: " << glfwGetJoystickName(jid) << "\n";
    else if (event == GLFW_DISCONNECTED)
        std::cout << "Joystick " << jid << " disconnected\n";
}
void Shell::OnJoystick(int, int){

}

void Shell::Cleanup(){

}
// === SETUP CALLBACKS ===

void Shell::InitCallbacks(GLFWwindow* window) {
    glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
    glfwSetKeyCallback(window, KeyPressCallback);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);
    glfwSetCursorPosCallback(window, CursorPositionCallback);
    glfwSetScrollCallback(window, ScrollCallback);
    glfwSetWindowFocusCallback(window, WindowFocusCallback);
    glfwSetDropCallback(window, DropCallback);
    glfwSetJoystickCallback(JoystickCallback);
}



