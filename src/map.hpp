#pragma once
#ifndef _MAP_HPP
#define _MAP_HPP

#include "vector.hpp"
#include <stddef.h>
#include <stdint.h>

#define LINEDEF_FLAG_TWO_SIDED 0x0004 // Flag indicating a two-sided linedef

typedef struct sector{
    int16_t floor, ceiling; // Floor and ceiling heights
    int16_t light_level; // Light level of the sector
    int floor_tex, ceiling_tex; // Textures for floor and ceiling
} sector_t;

typedef struct linedef{
    uint16_t start_idx, end_idx, flags, front_sidedef, back_sidedef; // Indices of the start and end vertices
    
} linedef_t;

typedef struct sidedef{
    uint16_t sector_idx; // Index of the sector this sidedef belongs to
} sidedef_t;

typedef struct map{
    vec2_t *vertices; // Array of vertices
    size_t num_vertices; // Number of vertices
    vec2_t min;      // Minimum coordinates (bounding box)
    vec2_t max;      // Maximum coordinates (bounding box)
    size_t     num_linedefs;
    linedef_t *linedefs;
    size_t num_sidedefs;
    sidedef_t *sidedefs; // Assuming sidedefs are similar to linedefs
    size_t num_sectors;
    sector_t *sectors; // Array of sectors
}map_t;



#endif // _MAP_HPP