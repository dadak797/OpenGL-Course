# ExternalProject 관련 명령어 셋 추가
include(ExternalProject)

# Dependency 관련 변수 설정
set(DEP_INSTALL_DIR ${PROJECT_BINARY_DIR}/install)
set(DEP_INCLUDE_DIR ${DEP_INSTALL_DIR}/include)
set(DEP_LIB_DIR ${DEP_INSTALL_DIR}/lib)

# spdlog: fast logger library
ExternalProject_Add(
    dep_spdlog
    GIT_REPOSITORY "https://github.com/gabime/spdlog.git"
    GIT_TAG "v1.x"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
    TEST_COMMAND ""
)

# Dependency 리스트 및 라이브러리 파일 리스트 추가
set(DEP_LIST ${DEP_LIST} dep_spdlog)
set(DEP_LIBS ${DEP_LIBS} spdlog$<$<CONFIG:Debug>:d>)

if(EMSCRIPTEN)
    message(STATUS "Emscripten used")
    set_target_properties(${PROJECT_NAME} 
        PROPERTIES SUFFIX ".html"
        LINK_FLAGS "-O3 -s USE_WEBGL2=1 -s FULL_ES3=1 -s USE_GLFW=3 -s WASM=1 -s ALLOW_MEMORY_GROWTH=1 -s NO_EXIT_RUNTIME=1 --shell-file ${CMAKE_SOURCE_DIR}/shell_minimal.html --preload-file ${CMAKE_SOURCE_DIR}@/"
    )
else()
    message(STATUS "Emscripten not used")
    # glfw
    ExternalProject_Add(
        dep_glfw
        GIT_REPOSITORY "https://github.com/glfw/glfw.git"
        GIT_TAG "3.3.3"
        GIT_SHALLOW 1
        UPDATE_COMMAND "" PATCH_COMMAND "" TEST_COMMAND ""
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
            -DGLFW_BUILD_EXAMPLES=OFF
            -DGLFW_BUILD_TESTS=OFF
            -DGLFW_BUILD_DOCS=OFF
    )

    # Dependency 리스트 및 라이브러리 파일 리스트 추가
    set(DEP_LIST ${DEP_LIST} dep_glfw)
    set(DEP_LIBS ${DEP_LIBS} glfw3)

    # glad
    ExternalProject_Add(
        dep_glad
        GIT_REPOSITORY "https://github.com/Dav1dde/glad.git"
        GIT_TAG "v0.1.36"
        GIT_SHALLOW 1
        UPDATE_COMMAND ""
        PATCH_COMMAND ""
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${DEP_INSTALL_DIR}
            -DGLAD_INSTALL=ON
        TEST_COMMAND ""
    )

    set(DEP_LIST ${DEP_LIST} dep_glad)
    set(DEP_LIBS ${DEP_LIBS} glad)
endif()

# stb
ExternalProject_Add(
    dep_stb
    GIT_REPOSITORY "https://github.com/nothings/stb"
    GIT_TAG "master"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy
        ${PROJECT_BINARY_DIR}/dep_stb-prefix/src/dep_stb/stb_image.h
        ${DEP_INSTALL_DIR}/include/stb/stb_image.h
)

set(DEP_LIST ${DEP_LIST} dep_stb)

# glm
ExternalProject_Add(
    dep_glm
    GIT_REPOSITORY "https://github.com/g-truc/glm"
    GIT_TAG "0.9.9.8"
    GIT_SHALLOW 1
    UPDATE_COMMAND ""
    PATCH_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    TEST_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_BINARY_DIR}/dep_glm-prefix/src/dep_glm/glm
        ${DEP_INSTALL_DIR}/include/glm
)

set(DEP_LIST ${DEP_LIST} dep_glm)

# ImGUI
set(IMGUI_VERSION "1.88")
if(EMSCRIPTEN)
    add_library(imgui
        imgui/imgui-emscripten/imgui_draw.cpp
        imgui/imgui-emscripten/imgui_widgets.cpp
        imgui/imgui-emscripten/imgui.cpp
        imgui/imgui-emscripten/imgui_impl_glfw.cpp
        imgui/imgui-emscripten/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/src)
    add_dependencies(imgui ${DEP_LIST})
    set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui-emscripten)
    set(DEP_LIST ${DEP_LIST} imgui)
    set(DEP_LIBS ${DEP_LIBS} imgui)
else()
    add_library(imgui
        imgui/imgui-${IMGUI_VERSION}/imgui_draw.cpp
        imgui/imgui-${IMGUI_VERSION}/imgui_tables.cpp
        imgui/imgui-${IMGUI_VERSION}/imgui_widgets.cpp
        imgui/imgui-${IMGUI_VERSION}/imgui.cpp
        imgui/imgui-${IMGUI_VERSION}/imgui_impl_glfw.cpp
        imgui/imgui-${IMGUI_VERSION}/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PRIVATE ${DEP_INCLUDE_DIR})
    add_dependencies(imgui ${DEP_LIST})
    set(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/imgui/imgui-${IMGUI_VERSION})
    set(DEP_LIST ${DEP_LIST} imgui)
    set(DEP_LIBS ${DEP_LIBS} imgui)
endif()