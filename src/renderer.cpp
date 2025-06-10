#include "renderer.hpp"
#include <fstream>
#include <sstream>

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
        glEnable(GL_DEPTH_TEST);
        init_quad();
        init_shader();
        init_projection();
    }

    void renderer::renderer_clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void renderer::renderer_set_projection(mat4_t projection) {
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.v);
    }

    vec2 renderer::renderer_get_size(){
        return (vec2){width, height};
    }

    void renderer::init_shader(){
        std::string vertex_src = load_shader_source("../res/shaders/vert.shader");
        std::string fragment_src = load_shader_source("../res/shaders/frag.shader");
        GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_src.c_str());
        GLuint fragment = compile_shader(GL_FRAGMENT_SHADER, fragment_src.c_str());
        program = link_program(2, vertex, fragment);

        glUseProgram(program);

        projection_location = glGetUniformLocation(program, "projection");
        model_location = glGetUniformLocation(program, "model");
        view_location = glGetUniformLocation(program, "view");
        color_location = glGetUniformLocation(program, "color");
        use_texture_location = glGetUniformLocation(program, "useTexture");
        GLuint palette_location = glGetUniformLocation(program, "palette");
        glUniform1i(palette_location, 0); // Texture unit 0 for palette texture
        GLuint texture_location = glGetUniformLocation(program, "tex");
        glUniform1i(texture_location, 1);
    }

    void renderer::renderer_set_draw_texture(GLuint texture) {
        if (texture == 0) {
            glUniform1i(use_texture_location, 0);
        } else {
            glUniform1i(use_texture_location, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texture);
        } 
    }

    void renderer::renderer_draw_mesh(mesh_t *mesh, mat4_t transformation, int color){
        glUniform1i(color_location, color);
        glUniformMatrix4fv(model_location, 1, GL_FALSE, transformation.v);
        glBindVertexArray(mesh->vao);
        glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, NULL);
    }

    void renderer::renderer_draw_point(vec2_t point, float size, int color){
        mat4_t translation = mat4_translate((vec3_t){point.x, point.y, 0.f});
        mat4_t scale = mat4_scale((vec3_t){size, size, 1.f});
        mat4_t model = mat4_mul(scale, translation);

        renderer_draw_mesh(&quad_mesh, model, color);
    }

    

    void renderer::renderer_draw_line(vec2_t p0, vec2_t p1, float width, int color) {
        float x = p1.x - p0.x, y = p0.y - p1.y;
        float r = sqrtf(x * x + y * y), angle = atan2f(y, x);

        mat4_t translation =
            mat4_translate((vec3_t){(p0.x + p1.x) / 2.f, (p0.y + p1.y) / 2.f, 0.f});
        mat4_t scale    = mat4_scale((vec3_t){r, width, 1.f});
        mat4_t rotation = mat4_rotate((vec3_t){0.f, 0.f, 1.f}, angle);
        mat4_t model    = mat4_mul(mat4_mul(scale, rotation), translation);

        renderer_draw_mesh(&quad_mesh, model, color);
    }

    void renderer::renderer_set_palette_texture(GLuint texture){
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_1D, texture);
    }

    void renderer::renderer_draw_quad(vec2_t center, vec2_t size, float angle, int color) {
        mat4_t translation = mat4_translate((vec3_t){center.x, center.y, 0.f});
        mat4_t scale       = mat4_scale((vec3_t){size.x, size.y, 1.f});
        mat4_t rotation    = mat4_rotate((vec3_t){0.f, 0.f, 1.f}, angle);
        mat4_t model       = mat4_mul(mat4_mul(scale, rotation), translation);

        renderer_draw_mesh(&quad_mesh, model, color);
    }

    void renderer::init_quad(){
        vertex_t vertices[] = {
            { .position = { 0.5f,  0.5f, 0.f }},   // top-right
            { .position = { 0.5f, -0.5f, 0.f }},   // bottom-right
            { .position = {-0.5f, -0.5f, 0.f }},   // bottom-left
            { .position = {-0.5f,  0.5f, 0.f }},   // top-left
        };

        uint32_t indices[] = {
            0, 1, 3,
            1, 2, 3
        };

        mesh_create(&quad_mesh, 4, vertices, 6, indices, false);
    }

    void renderer::renderer_set_viewport(mat4_t view){
        glUniformMatrix4fv(view_location, 1, GL_FALSE, view.v);
    }

    void renderer::init_projection(){
        mat4_t projection = mat4_ortho(0.f, width, height, 0.f, -1.f, 1.f);
        GLuint projection_location = glGetUniformLocation(program, "projection");
        glUniformMatrix4fv(projection_location, 1, GL_FALSE, projection.v);
        glUniformMatrix4fv(view_location, 1, GL_FALSE, mat4_identity().v);
    }

    std::string renderer::load_shader_source(const char* filepath) {
        std::ifstream file(filepath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
}