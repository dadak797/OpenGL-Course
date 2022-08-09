#include "context.h"
#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
#endif
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>


void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->Reshape(width, height);
}

#ifdef __EMSCRIPTEN__
EM_BOOL onResizeCallback(int theEventType, const EmscriptenUiEvent* theEvent, void* window)
{
    int width, height;
    emscripten_get_canvas_element_size("#canvas", &width, &height);
    OnFramebufferSizeChange((GLFWwindow*)window, width, height);
    return EM_FALSE;  
}
#endif

void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    SPDLOG_INFO("key: {}, scancode: {}, action: {}, mods: {}{}{}",
        key, scancode,
        action == GLFW_PRESS ? "Pressed" :
        action == GLFW_RELEASE ? "Released" :
        action == GLFW_REPEAT ? "Repeat" : "Unknown",
        mods & GLFW_MOD_CONTROL ? "C" : "-",
        mods & GLFW_MOD_SHIFT ? "S" : "-",
        mods & GLFW_MOD_ALT ? "A" : "-");
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void OnCursorPos(GLFWwindow* window, double x, double y) {
    ImGui_ImplGlfw_CursorPosCallback(window, x, y);
    auto context = (Context*)glfwGetWindowUserPointer(window);
    context->MouseMove(x, y);
}

void OnMouseButton(GLFWwindow* window, int button, int action, int modifier) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, modifier);
    auto context = (Context*)glfwGetWindowUserPointer(window);
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    context->MouseButton(button, action, x, y);
}

void OnCharEvent(GLFWwindow* window, unsigned int ch) {
    ImGui_ImplGlfw_CharCallback(window, ch);
}

void OnScroll(GLFWwindow* window, double xoffset, double yoffset) {
    ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
}

std::function<void()> loop;
void main_loop() { loop(); }
 
int main(int argc, const char** argv) 
{
    SPDLOG_INFO("Start program");

    // glfw 라이브러리 초기화, 실패하면 에러 출력 후 종료
    SPDLOG_INFO("Initialize glfw");
    if(!glfwInit()) {
    #ifdef __EMSCRIPTEN__
        SPDLOG_ERROR("failed to initialize glfw");
    #else
        const char* description = nullptr;
        glfwGetError(&description);
        SPDLOG_ERROR("failed to initialize glfw: {}", description);
    #endif
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성, 실패하면 에러 출력 후 종료
    SPDLOG_INFO("Create glfw window");
#ifdef __EMSCRIPTEN__
    int width, height;
    emscripten_get_canvas_element_size("#canvas", &width, &height);
    auto window = glfwCreateWindow(width, height, WINDOW_NAME, nullptr, nullptr);
#else
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME, nullptr, nullptr);
#endif
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwMakeContextCurrent(window); 

#ifdef __EMSCRIPTEN__
#else
    // glad를 활용한 OpenGL 함수 로딩
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        SPDLOG_ERROR("failed to initialize glad");
        glfwTerminate();
        return -1;
    }
#endif
    auto glVersion = glGetString(GL_VERSION);
    SPDLOG_INFO("OpenGL context version: {}", glVersion);
    
    /// OpenGL function can be used from here. ///

    // ImGUI Initialization
    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();
    io.Fonts->AddFontFromFileTTF("imgui/Fonts/Consolas.ttf", 13.0f);

    ImGui_ImplGlfw_InitForOpenGL(window, false);
    ImGui_ImplOpenGL3_Init();
    ImGui_ImplOpenGL3_CreateFontsTexture();
    ImGui_ImplOpenGL3_CreateDeviceObjects();

    auto context = Context::Create();
    if (!context) {
        SPDLOG_ERROR("failed to create context");
        glfwTerminate();
        return -1;
    }
    glfwSetWindowUserPointer(window, context.get());  // Get window size using user pointer

#ifdef __EMSCRIPTEN__
    emscripten_get_canvas_element_size("#canvas", &width, &height);
    OnFramebufferSizeChange(window, width, height);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, (void*)window, EM_TRUE, onResizeCallback);
#else
    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
#endif
    glfwSetKeyCallback(window, OnKeyEvent);
    glfwSetCharCallback(window, OnCharEvent);
    glfwSetCursorPosCallback(window, OnCursorPos);
    glfwSetMouseButtonCallback(window, OnMouseButton);
    glfwSetScrollCallback(window, OnScroll);

    loop = [&] {
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        context->ProcessInput(window);
        context->Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    };

    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(!glfwWindowShouldClose(window)) {
        main_loop();
    }
#endif
    context.reset();  // 메모리 정리

    ImGui_ImplOpenGL3_DestroyFontsTexture();
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext(imguiContext);

    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}