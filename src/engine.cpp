#include "engine.hpp"
#include "matrix.hpp"
#include <iostream>
#include <math.h>
#include "input.hpp" // Add this line if is_button_pressed is declared here, or declare the function below if not.

#define FOV (3.14159265358979323846 / 3.f)
#define PLAYER_SPEED (5.f)
#define MOUSE_SENSITIVITY (0.002f)
namespace silic {

engine::engine() {}

engine::~engine() {}

void engine::init(Input* in, wad_t* wad, const std::string& mapname, int width, int height) {
    wad_ptr = wad;
    input = in; // Copy the input object

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
    float speed = input->is_button_pressed(button::KEY_LSHIFT) ? PLAYER_SPEED * 2.f : PLAYER_SPEED;
    camera_update_direction_vectors(&camera);
    if(input->is_button_pressed(button::KEY_W)) {
        camera.position = vec3_add(camera.position, vec3_scale(camera.forward, speed * delta));
    }

    if(input->is_button_pressed(button::KEY_S)) {
        camera.position = vec3_add(camera.position, vec3_scale(camera.forward, -speed * delta));
    }

    if(input->is_button_pressed(button::KEY_A)) {
        camera.position = vec3_add(camera.position, vec3_scale(camera.right, -speed * delta));
    }

    if(input->is_button_pressed(button::KEY_D)) {
        camera.position = vec3_add(camera.position, vec3_scale(camera.right, speed * delta));
    }

    if(input->is_button_pressed(button::KEY_SPACE)) {
        camera.position = vec3_add(camera.position, vec3_scale(camera.up, speed * delta));
    }

    if(input->is_button_pressed(button::KEY_LEFT_CONTROL)) {
        camera.position = vec3_add(camera.position, vec3_scale(camera.up, -speed * delta));
    }

    if(input->is_button_pressed(button::MOUSE_LEFT)) {
        if(!input->is_mouse_captured()) {
            last_mouse_pos = input->get_mouse_position();
            input->set_mouse_captured(true);
        }
        vec2_t mouse_pos = input->get_mouse_position();
        float dx = mouse_pos.x - last_mouse_pos.x;
        float dy = mouse_pos.y - last_mouse_pos.y;
        last_mouse_pos = mouse_pos;
        camera.yaw -= dx * MOUSE_SENSITIVITY;
        camera.pitch -= dy * MOUSE_SENSITIVITY;
        if (camera.pitch > 1.5f) camera.pitch = 1.5f;
        if (camera.pitch < -1.5f) camera.pitch = -1.5f;
    }else if(input->is_mouse_captured()) {
        input->set_mouse_captured(false);
    }
}


void engine::render() {
    m_renderer.renderer_clear(); // 清屏
    mat4_t view = mat4_look_at(camera.position, vec3_add(camera.position, camera.forward), camera.up);
    m_renderer.renderer_set_viewport(view);
    m_renderer.renderer_draw_mesh(&quad_mesh, mat4_identity(), (vec4_t){1.f, 1.f, 1.f, 1.f});
}

} // namespace silic
