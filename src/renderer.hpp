#pragma once
#include <glad/glad.h>
#include "gl_helpers.hpp"
#include "matrix.hpp"
#include "vector.hpp"
#include <math.h>
#include <stdint.h>
#include "mesh.hpp"
#include <string>

namespace silic{
    class renderer{
        public:
            renderer();
            ~renderer();

            void init_shader();
            void init_projection();
            void init_quad();
            void renderer_init(int w, int h);
            void renderer_clear();
            void renderer_set_projection(mat4_t projection);
            void renderer_draw_mesh(mesh_t *mesh, mat4_t transformation, int color);
            void renderer_set_palette_texture(GLuint texture);
            void renderer_set_draw_texture(GLuint texture);
            void renderer_draw_point(vec2_t point, float size, int color);
            void renderer_draw_line(vec2_t p0, vec2_t p1, float width, int color);
            void renderer_draw_quad(vec2_t center, vec2_t size, float angle, int color);
            void renderer_set_viewport(mat4_t view);
            vec2 renderer_get_size();
            mesh_t quad_mesh;
            GLuint program;
            GLuint color_location;
            GLuint model_location;
            GLuint projection_location;
            GLuint view_location;
            GLuint use_texture_location;
                    
        private:
            float width, height;
            std::string load_shader_source(const char* filepath);
    };
}