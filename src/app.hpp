#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

namespace silic{
    class app{
        public:
        app();
        ~app();

        app(const app&) = delete;
        app& operator=(const app&) = delete;

        void run();
        GLFWwindow* window;

    private:
        static const int WIDTH = 800;
        static const int HEIGHT = 600;
        void processInput(GLFWwindow *window);
        void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    };
    
}