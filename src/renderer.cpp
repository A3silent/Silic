#include "renderer.hpp"

namespace silic{
    renderer::renderer(){
        //init a default width and height
        width = 800;
        height = 600;
    }

    renderer::~renderer(){

    }

    void renderer::init_shader(){
        GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        program = link_program(2, vertex, fragment);
    }

    void renderer::init_quad(){
        float vertices[] = {
            .5f, .5f, //
            .5f, -.5f,
            -.5f,-.5f,
            -.5f, .5f
        };

        uint32_t indices[] = {
            0, 1, 3,
            1, 2, 3
        };

        GLuint vao, vbo, ebo;
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        glBindVertexArray(vao);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void renderer::renderer_init(int w, int h){
        width = w;
        height = h;
        glClearColor(1.f, 1.f, 1.f, 1.f);
    }

    void renderer::renderer_clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }
}