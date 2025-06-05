#include <iostream>
#include "renderer.hpp"
#include "app.hpp"
#include "wad.hpp"
#include "engine.hpp"



namespace silic {

    app::app() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //create window
        window = glfwCreateWindow(WIDTH, HEIGHT, "Silic", NULL, NULL);
        glfwMakeContextCurrent(window);
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

    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    void app::load_map(){

    }

    //main function
    void app::run(std::string mapname) {

        //glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

        // 使用 engine 类替代原有 renderer/map/wad 逻辑
        silic::engine eng;

        wad_t wad;
        if(load_wad("../res/map/doom1.wad", &wad) != 0){
            std::cerr<<"Failed to load wad map"<<std::endl;
        }

        eng.init(&wad, mapname, WIDTH, HEIGHT);

        float angle = 0.f;
        char title[128];
        float last = 0.f;
        // render loop
        while (!glfwWindowShouldClose(window))
        {
            silic::processInput(window);
            float now   = glfwGetTime();
            float dTime = now - last;
            last        = now;
            angle += 1.2*dTime;

            eng.update(dTime);

            glfwPollEvents();
            snprintf(title, 128, "Silic || FPS: %.0f", 1.f/dTime);
            glfwSetWindowTitle(window, title);

            eng.render();

            glfwSwapBuffers(window);
        }
        glfwTerminate();
    }
}
