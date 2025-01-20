#pragma once
#include <glad/glad.h>
#include "gl_helpers.hpp"
#include "matrix.hpp"
#include "vector.hpp"
#include <math.h>
#include <stdint.h>

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
            void renderer_draw_line(vec2_t p0, vec2_t p1, float width, vec4_t color);
            GLuint program;
            GLuint color_location;
            GLuint model_location;
                    
        private:
            float width, height;
            const char *vertexShaderSource = 
            "#version 330 core\n"
            "layout(location = 0) in vec2 pos;\n"
            "uniform mat4 model;\n"
            "uniform mat4 projection;\n"
            "void main(){\n"
            "  gl_Position = projection * model * vec4(pos, 0.0, 1.0);\n"
            "}\n\0";

            const char *fragmentShaderSource = 
            "#version 330 core\n"
            "out vec4 fragColor;\n"
            "uniform vec4 color;\n"
            "void main(){\n"
            "  fragColor = color;\n"
            "}\n\0";
    };
}