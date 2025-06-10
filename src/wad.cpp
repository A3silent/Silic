#include "wad.hpp"
#include <fstream>
#include <math.h>
#include <memory>
#include <iostream>
#include <cstring>

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

    int wad_find_lump_32(std::string lumpname, const wad_t *wad) {
        std::string lumpname8 = lumpname.substr(0, 32);
        lumpname8.resize(32, '\0');
        for (int i = 0; i < wad->num_lumps; i++) {
            if (wad->lumps[i].name == lumpname8) {
                return i; 
            }
        }
        return -1;
    }

    int wad_read_playpal(palette_t *palette, const wad_t *wad){
        std::string lumpname = "PLAYPAL";
        int playpal_index = wad_find_lump(lumpname, wad);
        if(playpal_index < 0) {
            return 1; // PLAYPAL lump not found
        }

        memcpy(palette->color, wad->lumps[playpal_index].data.data(), NUM_COLORS * 3);

        return 0;
    }

    flat_texture_t* wad_read_flat_texture(size_t* num, const wad_t *wad){
        int f_start = wad_find_lump("F_START", wad);
        int f_end   = wad_find_lump("F_END", wad);

        if (num == NULL || f_end < 0 || f_start < 0) { return NULL; }


        *num              = f_end - f_start - 1;
        flat_texture_t *flats = new flat_texture_t[*num];

        for (int i = f_start + 1; i < f_end; i++) {
            if (wad->lumps[i].size != FLAT_TEXTURE_SIZE * FLAT_TEXTURE_SIZE) {
            (*num)--;
            continue;
            }
            memcpy(flats[i - f_start - 1].pixels, wad->lumps[i].data.data(),
                    FLAT_TEXTURE_SIZE * FLAT_TEXTURE_SIZE);
        }

        return flats;
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
        map->vertices     = new vec2_t[map->num_vertices];

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
        map->linedefs     = new linedef_t[map->num_linedefs];

        for (int i = 0, j = 0; i < lump->size; i += 14, j++) {
            map->linedefs[j].start_idx     = READ_I16(lump->data, i);
            map->linedefs[j].end_idx       = READ_I16(lump->data, i + 2);
            map->linedefs[j].flags         = READ_I16(lump->data, i + 4);
            map->linedefs[j].front_sidedef = READ_I16(lump->data, i + 10);
            map->linedefs[j].back_sidedef  = READ_I16(lump->data, i + 12);
        }
    }

    void read_sidedefs(map_t *map, const lump_t *lump){
        map->num_sidedefs = lump->size / 30; // each sidedef is 30 bytes
        map->sidedefs = new sidedef_t[map->num_sidedefs];
        
        for (int i = 0, j = 0; i < lump->size; i += 30, j++) {
            map->sidedefs[j].sector_idx = READ_I16(lump->data, i + 28);
        }
    }

    void read_sectors(map_t *map, const lump_t *lump, const wad_t *wad){
        map->num_sectors = lump->size / 26; // each sector is 26 bytes
        map->sectors = new sector_t[map->num_sectors];

        int f_start = wad_find_lump("F_START", wad);
        int f_end   = wad_find_lump("F_END", wad);

        for (int i = 0, j = 0; i < lump->size; i += 26, j++) {
            map->sectors[j].floor   = (int16_t)READ_I16(lump->data, i);
            map->sectors[j].ceiling = (int16_t)READ_I16(lump->data, i + 2);
            map->sectors[j].light_level = (int16_t)READ_I16(lump->data, i + 20);

            char name[9] = {0};
            memcpy(name, &lump->data[i + 4], 8);
            int floor = wad_find_lump(name, wad);
            if (floor <= f_start || floor >= f_end) {
                floor = -1;
            } else {
                map->sectors[j].floor_tex = floor - f_start - 1; // Adjust index to match flat texture array
            }

            memcpy(name, &lump->data[i + 12], 8);
            int ceiling = wad_find_lump(name, wad);
            if (ceiling <= f_start || ceiling >= f_end) {
                ceiling = -1;
            } else {
                map->sectors[j].ceiling_tex = ceiling - f_start - 1; // Adjust index to match flat texture array
            }
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
        read_sidedefs(map, &wad->lumps[map_index + SIDEDEFS_IDX]);
        read_sectors(map, &wad->lumps[map_index + SECTORS_IDX], wad);
        return 0; // Success
    }

    #define GL_VERTICES_IDX 1
    #define GL_SEGS_IDX     2
    #define GL_SSECTORS_IDX 3

    int wad_read_gl_map(std::string gl_mapname, gl_map_t *map, const wad_t *wad) {
        int map_index = wad_find_lump(gl_mapname, wad);
        if (map_index < 0) { return 1; }

        if (strncmp(reinterpret_cast<const char*>(wad->lumps[map_index + GL_VERTICES_IDX].data.data()),
                    "gNd2", 4) != 0) {
            return 2;
        }

        if (strncmp(reinterpret_cast<const char*>(wad->lumps[map_index + GL_SEGS_IDX].data.data()), "gNd3",
                    4) == 0) {
            return 2;
        }
        read_gl_vertices(map, &wad->lumps[map_index + GL_VERTICES_IDX]);
        read_gl_segments(map, &wad->lumps[map_index + GL_SEGS_IDX]);
        read_gl_subsectors(map, &wad->lumps[map_index + GL_SSECTORS_IDX]);
        //read_gl_nodes(map, &wad->lumps[map_index + GL_NODES_IDX]);

        return 0;
    }

    void read_gl_vertices(gl_map_t *map, const lump_t *lump){
        map->num_vertices = (lump->size - 4) / 8; // each vertex is 4+4=8 bytes
        map->vertices     = new vec2_t[map->num_vertices];

        map->min = (vec2_t){INFINITY, INFINITY};
        map->max = (vec2_t){-INFINITY, -INFINITY};

        for (int i = 4, j = 0; i < lump->size; i += 8, j++) {
            map->vertices[j].x = (float)((int32_t)READ_I32(lump->data, i)) / (1 << 16);
            map->vertices[j].y = (float)((int32_t)READ_I32(lump->data, i + 4)) / (1 << 16);

            if (map->vertices[j].x < map->min.x) { map->min.x = map->vertices[j].x; }
            if (map->vertices[j].y < map->min.y) { map->min.y = map->vertices[j].y; }
            if (map->vertices[j].x > map->max.x) { map->max.x = map->vertices[j].x; }
            if (map->vertices[j].y > map->max.y) { map->max.y = map->vertices[j].y; }
        }
    }

    void read_gl_subsectors(gl_map_t *map, const lump_t *lump) {
        map->num_subsectors = lump->size / 4; // each subsector is 4 bytes
        map->subsectors     = new gl_subsector_t[map->num_subsectors];

        for (int i = 0, j = 0; i < lump->size; i += 4, j++) {
            map->subsectors[j].num_segs  = READ_I16(lump->data, i);
            map->subsectors[j].first_seg = READ_I16(lump->data, i + 2);
        }
    }

    void read_gl_segments(gl_map_t *map, const lump_t *lump){
        map->num_segments = lump->size / 10; // each segment is 10 bytes
        map->segments     = new gl_segment_t[map->num_segments];

        for (int i = 0, j = 0; i < lump->size; i += 10, j++) {
            map->segments[j].start_vertex = READ_I16(lump->data, i);
            map->segments[j].end_vertex   = READ_I16(lump->data, i + 2);
            map->segments[j].linedef      = READ_I16(lump->data, i + 4);
            map->segments[j].side         = READ_I16(lump->data, i + 6);
        }
    }

    
    void read_gl_nodes(gl_map_t *map, const lump_t *lump) {
        // map->num_nodes = lump->size / 28; // each node is 28 bytes
        // map->nodes     = new gl_node_t[map->num_nodes];

        // for (int i = 0, j = 0; i < lump->size; i += 28, j++) {
        //     map->nodes[j].partition.x       = (int16_t)READ_I16(lump->data, i);
        //     map->nodes[j].partition.y       = (int16_t)READ_I16(lump->data, i + 2);
        //     map->nodes[j].delta_partition.x = (int16_t)READ_I16(lump->data, i + 4);
        //     map->nodes[j].delta_partition.y = (int16_t)READ_I16(lump->data, i + 6);

        //     map->nodes[j].front_bbox[0] = READ_I16(lump->data, i + 8);
        //     map->nodes[j].front_bbox[1] = READ_I16(lump->data, i + 10);
        //     map->nodes[j].front_bbox[2] = READ_I16(lump->data, i + 12);
        //     map->nodes[j].front_bbox[3] = READ_I16(lump->data, i + 14);

        //     map->nodes[j].back_bbox[0] = READ_I16(lump->data, i + 16);
        //     map->nodes[j].back_bbox[1] = READ_I16(lump->data, i + 18);
        //     map->nodes[j].back_bbox[2] = READ_I16(lump->data, i + 20);
        //     map->nodes[j].back_bbox[3] = READ_I16(lump->data, i + 22);

        //     map->nodes[j].front_child_id = READ_I16(lump->data, i + 24);
        //     map->nodes[j].back_child_id  = READ_I16(lump->data, i + 26);
        // }
    }

    void wad_free_gl_map(gl_map_t *map) {
    }

}