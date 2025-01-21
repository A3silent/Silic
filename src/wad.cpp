#include "wad.hpp"
#include <fstream>
#include <memory>
#include <iostream>

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
    }
}