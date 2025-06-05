#pragma once

#include "wad.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "mesh.hpp"
#include <glad/glad.h>
#include <string>

namespace silic {

class engine {
public:
    engine();
    ~engine();

    void init(wad_t* wad, const std::string& mapname, int width, int height);
    void update(float delta);
    void render();

    camera_t camera;
    renderer m_renderer;
    vec2 size;

private:
    map_t map;
    wad_t* wad_ptr = nullptr;
    mesh_t quad_mesh;
};

} // namespace silic
