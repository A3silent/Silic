#pragma once

#include "wad.hpp"
#include "camera.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include "mesh.hpp"
#include <glad/glad.h>
#include <string>

namespace silic {

typedef struct wall_node {
    mat4_t model;
    const sector_t* sector;
    struct wall_node* next;

} wall_node_t, wall_list_t;

typedef struct flat_node {
    mesh_t mesh;
    const sector_t* sector;
    struct flat_node* next;

} flat_node_t, flat_list_t;


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
    vec3_t get_random_color(const void* seed);
    mat4_t model_from_vertices(vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3);
    void generate_meshes(const map_t *map, const gl_map_t* gl_map);
    map_t map;
    gl_map_t gl_map;
    vec2 last_mouse_pos;
    wad_t* wad_ptr = nullptr;
    mesh_t quad_mesh;
    Input* input = nullptr;
    wall_list_t* wall_list = nullptr;
    flat_list_t* flat_list = nullptr; 
    palette_t palette;
};

} // namespace silic
