#pragma once

#include "wad.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include <glad/glad.h>
#include <string>

namespace silic {

class engine {
public:
    engine();
    ~engine();

    void init(Input* in, wad_t* wad, const std::string& mapname, int width, int height);
    void update(float delta);
    void render();

    camera_t camera;
    renderer m_renderer;
    vec2 size;

private:
    map_t map;
    vec2 last_mouse_pos;
    wad_t* wad_ptr = nullptr;
    mesh_t quad_mesh;
    Input* input = nullptr;
};

} // namespace silic
