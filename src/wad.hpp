#pragma once
#include <string>
#include <vector>
#include <cstdint>

#include "map.hpp"

struct lump_t{
    std::string name;
    uint32_t offset;
    uint32_t size;
    std::vector<uint8_t> data; 
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
    int wad_find_lump(std::string lumpname, const wad_t *wad);
    void read_vertices(map_t *map, const lump_t *lump);
    void read_linedefs(map_t *map, const lump_t *lump);
    void read_sidedefs(map_t *map, const lump_t *lump);
    void read_sectors(map_t *map, const lump_t *lump);
}