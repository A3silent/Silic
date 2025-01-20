#include "renderer.hpp"

namespace silic{

    renderer::renderer(){
        //init a default width and height
        width = 800;
        height = 600;
    }

    renderer::~renderer(){

    }

    void renderer::renderer_init(int w, int h){
        width = w;
        height = h;
        glClearColor(.2f, .2f, .2f, 1.f);
        init_quad();
        init_shader();
        init_projection();
    }

    void renderer::renderer_clear() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void renderer::init_shader(){
        GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertexShaderSource);
        GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragmentShaderSource);
        program = link_program(2, vertex, fragment);

        glUseProgram(program);

        model_location = glGetUniformLocation(program, "model");
        color_location = glGetUniformLocation(program, "color");
    }

    void renderer::renderer_draw_point(vec2_t point, float size,vec4_t color){
        mat4_t translation = mat4_translate((vec3_t){point.x, point.y, 0.f});
        mat4_t scale = mat4_scale((vec3_t){size, size, 1.f});
        mat4_t model = mat4_mul(scale, translation);


        glUniform4fv(color_location, 1, color.v);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model.v);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
    }

    void renderer::renderer_draw_line(vec2_t p0, vec2_t p1, float width, vec4_t color) {
        float x = p1.x - p0.x, y = p0.y - p1.y;
        float r = sqrtf(x * x + y * y), angle = atan2f(y, x);

        mat4_t translation =
            mat4_translate((vec3_t){(p0.x + p1.x) / 2.f, (p0.y + p1.y) / 2.f, 0.f});
        mat4_t scale    = mat4_scale((vec3_t){r, width, 1.f});
        mat4_t rotation = mat4_rotate((vec3_t){0.f, 0.f, 1.f}, angle);
        mat4_t model    = mat4_mul(mat4_mul(scale, rotation), translation);

        glUniform4fv(color_location, 1, color.v);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, model.v);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
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
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                    GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void renderer::init_projection(){
        mat4_t projection = mat4_ortho(0.f, width, height, 0.f, -1.f, 1.f);
        GLuint projection_location = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.v);
    }
}