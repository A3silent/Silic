#pragma once
#include <glad/glad.h>
#include <stddef.h>

namespace silic{
    GLuint compile_shader(GLenum type, const char *src);
    GLuint link_program(size_t num_shaders, ...);
}