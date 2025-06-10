#pragma once

#include <stdint.h>
#include <glad/glad.h>

#define FLAT_TEXTURE_SIZE (64)

typedef struct flat_texture {
    uint8_t pixels[FLAT_TEXTURE_SIZE * FLAT_TEXTURE_SIZE];
} flat_texture_t;

GLuint generate_flat_texture(const flat_texture_t* flat);