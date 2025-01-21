#include <iostream>
#include "renderer.hpp"
#include "app.hpp"
#include "wad.hpp"



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

    // glfw: whenever the window size changed (by OS or user resize) this callback function executes
    void framebuffer_size_callback(GLFWwindow* window, int width, int height)
    {
        // make sure the viewport matches the new window dimensions; note that width and 
        // height will be significantly larger than specified on retina displays.
        glViewport(0, 0, width, height);
    }

    void app::load_map(){

    }

    //main function
    void app::run() {

        //glad: load all OpenGL function pointers
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
        }

            
        silic::renderer m_renderer;
        m_renderer.renderer_init(WIDTH, HEIGHT);

        //wad map
        wad_t wad;
        if(load_wad("../res/map/doom1.wad", &wad) != 0){
            std::cerr<<"Failed to load wad map"<<std::endl;
        }

        std::cout<<"Loaded the wad map: "<<wad.id<<" num_lumps: "<<wad.num_lumps<<" dirctory: "<<wad.directory_offset<<std::endl;

        std::cout<<"Lumps:"<<std::endl;
        for(int i = 0; i < wad.num_lumps; i++){
            std::cout<<wad.lumps[i].name<<" "<<wad.lumps[i].offset<<" "<<wad.lumps[i].size<<" "<<std::endl;
        }

        float angle = 0.f;
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
            //spin
            angle += 1.2*dTime;

            //Window
            glfwPollEvents();
            snprintf(title, 128, "Silic || FPS: %.0f", 1.f/dTime);
            glfwSetWindowTitle(window, title);

            m_renderer.renderer_clear();
            m_renderer.renderer_draw_point((vec2_t){WIDTH/2.f, HEIGHT/2.f}, 5.f, (vec4_t){1.f, 0.f, 0.f, 1.f});
            m_renderer.renderer_draw_line((vec2_t){0.f, 0.f}, (vec2_t){WIDTH, HEIGHT}, 5.f, (vec4_t){0.f, 0.f, 1.f, 1.f});
            m_renderer.renderer_draw_line((vec2_t){WIDTH, 0.f}, (vec2_t){0.f, HEIGHT}, 5.f, (vec4_t){0.f, 0.f, 1.f, 1.f});
            m_renderer.renderer_draw_quad((vec2_t){100.f, 100.f}, (vec2_t){40.f, 40.f}, PI/4 + angle, (vec4_t){1.f, 1.f, 0.f, 1.f});

            glfwSwapBuffers(window);
        }
        glfwTerminate();
    }
}
