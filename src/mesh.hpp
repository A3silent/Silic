#pragma once

#ifndef _MESH_H
#define _MESH_H

#include <glad/glad.h>
#include <stddef.h>
#include <stdint.h>
#include "vector.hpp"

namespace silic {

typedef struct mesh {
  GLuint vao, vbo, ebo;
  size_t num_indices;
} mesh_t;

typedef struct vertex {
  vec3_t position;
} vertex_t;

typedef enum vertex_layout {
  VERTEX_LAYOUT_PLAIN,
  VERTEX_LAYOUT_FULL,
} vertex_layout_t;

void mesh_create(mesh_t *mesh,
                 size_t num_vertices, const void *vertices, size_t num_indices,
                 const uint32_t *indices, bool is_dynamic);


}
#endif