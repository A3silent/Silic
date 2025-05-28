#include "wad.hpp"
#include <fstream>
#include <math.h>
#include <memory>
#include <iostream>

#define READ_I16(buffer, offset)                                               \
  ((buffer)[(offset)] | ((buffer)[(offset + 1)] << 8))

#define READ_I32(buffer, offset)                                               \
  ((buffer)[(offset)] | ((buffer)[(offset + 1)] << 8) |                        \
   ((buffer)[(offset + 2)] << 16) | ((buffer)[(offset + 3)] << 24))

namespace silic{
    int load_wad(const char*filename, wad_t* wad) {
        if (wad == nullptr) {
            return 1; // Invalid wad pointer
        }

        // Open the file in binary mode
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            return 1; // File could not be opened
        }

        // Get the file size
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        if (size < 12) {
            return 3; // File too small to be a valid WAD
        }

        // Allocate buffer and read file contents
        std::vector<uint8_t> buffer(size);
        if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            return 1; // Error reading the file
        }

        // Extract WAD ID
        wad->id = std::string(buffer.begin(), buffer.begin() + 4);
        wad->num_lumps = READ_I32(buffer, 4);
        wad->directory_offset = READ_I32(buffer, 8);

        wad->lumps.resize(wad->num_lumps);
        for (int i = 0; i < wad->num_lumps; i++) {
            uint32_t offset = wad->directory_offset + i * 16;

            wad->lumps[i].offset = READ_I32(buffer, offset);
            wad->lumps[i].size = READ_I32(buffer, offset + 4);
            wad->lumps[i].name = std::string(buffer.begin() + offset + 8, buffer.begin() + offset + 16);
            wad->lumps[i].data.assign(buffer.begin() + wad->lumps[i].offset, buffer.begin() + wad->lumps[i].offset + wad->lumps[i].size);

        }


        return 0; // Success
    }

    void wad_free(wad_t *wad){
        if(wad == nullptr) return;
        // Clear the lumps vector and reset the wad structure
        wad->lumps.clear();
        wad->id.clear();
        wad->num_lumps = 0;
    }

    int wad_find_lump(std::string lumpname, const wad_t *wad) {
        std::string lumpname8 = lumpname.substr(0, 8);
        lumpname8.resize(8, '\0');
        for (int i = 0; i < wad->num_lumps; i++) {
            if (wad->lumps[i].name == lumpname8) {
                return i; 
            }
        }
        return -1;
    }

    #define THINGS_IDX   1
    #define LINEDEFS_IDX 2
    #define SIDEDEFS_IDX 3
    #define VERTEXES_IDX 4
    #define SEGS_IDX     5
    #define SSECTORS_IDX 6
    #define NODES_IDX    7
    #define SECTORS_IDX  8

    void read_vertices(map_t *map, const lump_t *lump) {
        map->num_vertices = lump->size / 4; // each vertex is 2+2=4 bytes
        map->vertices     = (vec2_t*)malloc(sizeof(vec2_t) * map->num_vertices);

        for (int i = 0, j = 0; i < lump->size; i += 4, j++) {
            map->vertices[j].x = (int16_t)READ_I16(lump->data, i);
            map->vertices[j].y = (int16_t)READ_I16(lump->data, i + 2);

            if (map->vertices[j].x < map->min.x) { map->min.x = map->vertices[j].x; }
            if (map->vertices[j].y < map->min.y) { map->min.y = map->vertices[j].y; }
            if (map->vertices[j].x > map->max.x) { map->max.x = map->vertices[j].x; }
            if (map->vertices[j].y > map->max.y) { map->max.y = map->vertices[j].y; }
        }
    }

    void read_linedefs(map_t *map, const lump_t *lump){
        map->num_linedefs = lump->size / 14; // each linedef is 14 bytes
        map->linedefs     = (linedef_t*)malloc(sizeof(linedef_t) * map->num_linedefs);

        for (int i = 0, j = 0; i < lump->size; i += 14, j++) {
            map->linedefs[j].start_idx     = READ_I16(lump->data, i);
            map->linedefs[j].end_idx       = READ_I16(lump->data, i + 2);
            map->linedefs[j].flags         = READ_I16(lump->data, i + 4);
            map->linedefs[j].front_sidedef = READ_I16(lump->data, i + 10);
            map->linedefs[j].back_sidedef  = READ_I16(lump->data, i + 12);
        }
    }

    int wad_read_map(std::string mapname, map_t *map, const wad_t *wad){
        int map_index = wad_find_lump(mapname, wad);
        map->min = (vec2_t){INFINITY, INFINITY};
        map->max = (vec2_t){-INFINITY, -INFINITY};
        if (map_index < 0) {
            return 1; // Map not found in the WAD
        }
        std::cout << mapname <<" found at index: " << map_index << std::endl;
        read_vertices(map, &wad->lumps[map_index + VERTEXES_IDX]);
        read_linedefs(map, &wad->lumps[map_index + LINEDEFS_IDX]);
        return 0; // Success
    }


}