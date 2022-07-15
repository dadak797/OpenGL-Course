#ifndef __COMMON_H__
#define __COMMON_H__


#include <memory>
#include <string>
#include <optional>
#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    //#define GL_GLEXT_PROTOTYPES
    //#define EGL_EGLEXT_PROTOTYPES
    #define GLFW_INCLUDE_ES3
#else  
    #include <glad/glad.h>
#endif
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

#define CLASS_PTR(klassName) \
    class klassName; \
    using klassName ## UPtr = std::unique_ptr<klassName>; \
    using klassName ## Ptr = std::shared_ptr<klassName>; \
    using klassName ## WPtr = std::weak_ptr<klassName>;


std::optional<std::string> LoadTextFile(const std::string& filename);


#endif // __COMMON_H__