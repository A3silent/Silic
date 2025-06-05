#include "engine.hpp"
#include "matrix.hpp"
#include <iostream>
#include <math.h>

#define FOV (3.14159265358979323846 / 3.f)

namespace silic {

engine::engine() {}

engine::~engine() {}

void engine::init(wad_t* wad, const std::string& mapname, int width, int height) {
    wad_ptr = wad;

    // 初始化渲染器
    m_renderer.renderer_init(width, height);

    camera = {
        .position = {0.f, 0.f, 3.f},
        .yaw = - (3.14159265358979323846 / 2.f),
        .pitch = 0.f,
    };

    size = m_renderer.renderer_get_size();
    mat4_t projection = mat4_perspective(FOV, size.x/size.y, .1f, 1000.f);
    m_renderer.renderer_set_projection(projection);

    if (wad_read_map(mapname, &map, wad) != 0) {
        std::cerr << "Failed to read " << mapname << " from wad" << std::endl;
        return;
    }
    
    float vertices[] = {
        .5f, .5f, 0.f, //
        .5f, -.5f, 0.f,
        -.5f,-.5f, 0.f,
        -.5f, .5f, 0.f
    };  

    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    mesh_create(&quad_mesh, 4, vertices, 6, indices, false);
}

void engine::update(float delta) {
    camera_update_direction_vectors(&camera);
}

void engine::render() {
    m_renderer.renderer_clear(); // 清屏
    mat4_t view = mat4_look_at(camera.position, vec3_add(camera.position, camera.forward), camera.up);
    m_renderer.renderer_set_viewport(view);
    m_renderer.renderer_draw_mesh(&quad_mesh, mat4_identity(), (vec4_t){1.f, 1.f, 1.f, 1.f});
}

} // namespace silic
