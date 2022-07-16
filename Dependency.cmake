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