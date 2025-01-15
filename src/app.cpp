#include "app.hpp"
#include <iostream>
#include "renderer.hpp"

namespace silic {

    app::app() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //create window
        window = glfwCreateWindow(WIDTH, HEIGHT, "Silic", NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
    }

    app::~app(){

    }

    void processInput(GLFWwindow *window)
    {
        if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
    }

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    //main function
    void app::run() {
        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, silic::framebuffer_size_callback);

        //glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

            
        silic::renderer m_renderer;
        m_renderer.renderer_init(WIDTH, HEIGHT);


        char title[128];
        float last = 0.f;
        // render loop
        while (!glfwWindowShouldClose(window))
        {
            // input
            silic::processInput(window);

            //fps
            float now   = glfwGetTime();
            float dTime = now - last;
            last        = now;

            //Window
            glfwPollEvents();
            snprintf(title, 128, "Silic || FPS: %.0f", 1.f/dTime);
            glfwSetWindowTitle(window, title);

            m_renderer.renderer_clear();
            m_renderer.renderer_draw_point((vec2_t){100.f, 100.f}, 5.f, (vec4_t){1.f, 1.f, 1.f, 1.f});
            glfwSwapBuffers(window);
        }
        glfwTerminate();
    }
}
