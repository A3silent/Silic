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
    void app::run(std::string mapname) {

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
        map_t map;
        if(wad_read_map(mapname, &map, &wad) != 0){
            std::cerr<<"Failed to read "<< mapname <<" from wad"<<std::endl;
        }

        // for(int i = 0; i < map.num_vertices; i++){
        //     std::cout<<"Vertex "<<i<<": "<<map.vertices[i].x<<", "<<map.vertices[i].y<<std::endl;
        // }

        // === coord mapping begin ===
        // 20 pixels margin
        vec2_t out_min = {20.f, 20.f};
        vec2_t out_max = {WIDTH - 20.f, HEIGHT - 20.f};
        vec2_t* remapped_vertices = (vec2_t*)malloc(sizeof(vec2_t) * map.num_vertices);
        for (size_t i = 0; i < map.num_vertices; i++) {
            // X axis mapping
            remapped_vertices[i].x =
                (std::max(map.min.x, std::min(map.vertices[i].x, map.max.x)) - map.min.x) *
                (out_max.x - out_min.x) / (map.max.x - map.min.x) + out_min.x;
            // Y axis mapping
            remapped_vertices[i].y = HEIGHT -
                ((std::max(map.min.y, std::min(map.vertices[i].y, map.max.y)) - map.min.y) *
                (out_max.y - out_min.y) / (map.max.y - map.min.y) + out_min.y);
        }
        //

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
            for(size_t i = 0; i < map.num_linedefs; i++){
                vec2_t start = remapped_vertices[map.linedefs[i].start_idx];
                vec2_t end   = remapped_vertices[map.linedefs[i].end_idx];
                m_renderer.renderer_draw_line(start, end, 1.f, (vec4_t){1.f, 0.f, 0.f, 1.f});
            }

            for(size_t i = 0; i < map.num_vertices; i++){
                m_renderer.renderer_draw_point(remapped_vertices[i], 3.f, (vec4_t){1.f, 1.f, 0.f, 1.f});
            }

            glfwSwapBuffers(window);
        }
        glfwTerminate();
    }
}
