#pragma once
#ifndef _MAP_HPP
#define _MAP_HPP

#include "vector.hpp"
#include <stddef.h>
#include <stdint.h>

typedef struct linedef{
    uint16_t start_idx, end_idx, flags, front_sidedef, back_sidedef; // Indices of the start and end vertices
    
} linedef_t;

typedef struct map{
    vec2_t *vertices; // Array of vertices
    size_t num_vertices; // Number of vertices
    vec2_t min;      // Minimum coordinates (bounding box)
    vec2_t max;      // Maximum coordinates (bounding box)
    size_t     num_linedefs;
    linedef_t *linedefs;
    // Add other map-related data structures as needed
}map_t;



#endif // _MAP_HPP