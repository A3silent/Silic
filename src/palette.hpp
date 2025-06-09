#pragma once

#include <stdint.h>
#include <glad/glad.h>

#define NUM_COLORS (256)


typedef struct palette {
    uint8_t color[256 * 3];
} palette_t;

GLuint color_create_texture(const palette *palette);

