#pragma once

#include <stddef.h>
#include <stdint.h>

#include "vector.hpp"

#define VERT_IS_GL (1 << 15)


typedef struct gl_subsector {
  uint16_t num_segs;
  uint16_t first_seg;
} gl_subsector_t;

typedef struct gl_segment {
  uint16_t start_vertex, end_vertex;
  uint16_t linedef, side;
} gl_segment_t;

typedef struct gl_map {
  size_t  num_vertices;
  vec2_t *vertices;
  vec2_t  min, max;

  size_t        num_segments;
  gl_segment_t *segments;

  size_t          num_subsectors;
  gl_subsector_t *subsectors;
} gl_map_t;
