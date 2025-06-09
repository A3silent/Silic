#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "map.hpp"
#include "gl_map.hpp"
#include "palette.hpp"

struct lump_t{
    std::string name;
    std::vector<uint8_t> data; 
    uint32_t size;
    uint32_t offset;
};

struct wad_t {
    std::string id; // Use std::string for automatic memory management
    uint32_t num_lumps;
    uint32_t directory_offset;
    std::vector<lump_t> lumps;   
};


namespace silic{
    int load_wad(const char*filename, wad_t *wad);
    void wad_free(wad_t *wad);
    int wad_read_map(std::string mapname, map_t *map, const wad_t *wad);
    int wad_read_gl_map(std::string gl_mapname, gl_map_t *map, const wad_t *wad);
    int wad_find_lump(std::string lumpname, const wad_t *wad);
    int wad_find_lump_32(std::string lumpname, const wad_t *wad);
    void read_vertices(map_t *map, const lump_t *lump);
    void read_linedefs(map_t *map, const lump_t *lump);
    void read_sidedefs(map_t *map, const lump_t *lump);
    void read_sectors(map_t *map, const lump_t *lump);
    void read_gl_vertices(gl_map_t *map, const lump_t *lump);
    void read_gl_segments(gl_map_t *map, const lump_t *lump);
    void read_gl_subsectors(gl_map_t *map, const lump_t *lump);
    void read_gl_nodes(gl_map_t *map, const lump_t *lump);
    void wad_free_gl_map(gl_map_t *map);
    int wad_read_playpal(palette_t *palette, const wad_t *wad);
}