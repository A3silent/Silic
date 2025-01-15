#pragma once
#include <glad/glad.h>
#include "gl_helpers.hpp"
#include "matrix.hpp"

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
            void renderer_draw_point(vec2_t point, float size, vec4_t color);

            GLuint program;
            GLuint model_location;

        private:
            int width, height;
            const char *vertexShaderSource = 
            "#version 330 core\n"
            "layout (location = 0) in vec3 aPos;\n"
            "void main()\n"
            "{\n"
            "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
            "}\0";

            const char *fragmentShaderSource = 
            "#version 330 core\n"
            "out vec4 FragColor;\n"
            "void main()\n"
            "{\n"
            "   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
            "}\n\0";
    };
}