#include "program.h"


void OnFramebufferSizeChange(GLFWwindow* window, int width, int height) {
    SPDLOG_INFO("framebuffer size changed: ({} x {})", width, height);
    glViewport(0, 0, width, height);
}

void OnKeyEvent(GLFWwindow* window,
    int key, int scancode, int action, int mods) {
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

std::function<void()> loop;
void main_loop() { loop(); }

int main(int argc, const char** argv) 
{
    SPDLOG_INFO("Start program");

    // glfw 라이브러리 초기화, 실패하면 에러 출력 후 종료
    SPDLOG_INFO("Initialize glfw");
    if(!glfwInit()) {
        //const char* description = nullptr;
        //glfwGetError(&description);
        //SPDLOG_ERROR("failed to initialize glfw: {}", description);
        //return -1;
        SPDLOG_ERROR("Failed to initialize glfw");
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw 윈도우 생성, 실패하면 에러 출력 후 종료
    SPDLOG_INFO("Create glfw window");
    auto window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_NAME,
                                   nullptr, nullptr);
    if (!window) {
        SPDLOG_ERROR("failed to create glfw window");
        glfwTerminate();
        //return -1;
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

#ifdef __EMSCRIPTEN__
    ShaderPtr vertexShader = Shader::CreateFromFile("./shader/simple_wasm.vs", GL_VERTEX_SHADER);
    ShaderPtr fragmentShader = Shader::CreateFromFile("./shader/simple_wasm.fs", GL_FRAGMENT_SHADER);
#else
    ShaderPtr vertexShader = Shader::CreateFromFile("./shader/simple.vs", GL_VERTEX_SHADER);
    ShaderPtr fragmentShader = Shader::CreateFromFile("./shader/simple.fs", GL_FRAGMENT_SHADER);
#endif
    SPDLOG_INFO("vertex shader id: {}", vertexShader->Get());
    SPDLOG_INFO("fragment shader id: {}", fragmentShader->Get());

    auto program = Program::Create({ fragmentShader, vertexShader });
    SPDLOG_INFO("program id: {}", program->Get());

    OnFramebufferSizeChange(window, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSetFramebufferSizeCallback(window, OnFramebufferSizeChange);
    glfwSetKeyCallback(window, OnKeyEvent);

    loop = [&] {
        glfwPollEvents();
        glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    };

    // glfw 루프 실행, 윈도우 close 버튼을 누르면 정상 종료
    SPDLOG_INFO("Start main loop");
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, true);
#else
    while(!glfwWindowShouldClose(window)) {
        // glfwPollEvents();
        // glClearColor(0.0f, 0.1f, 0.2f, 0.0f);
        // glClear(GL_COLOR_BUFFER_BIT);
        // glfwSwapBuffers(window);
        main_loop();
    }
#endif

    glfwDestroyWindow(window);
    glfwTerminate();
    //return 0;
    exit(EXIT_SUCCESS);
}