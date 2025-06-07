#include "engine.hpp"
#include "matrix.hpp"
#include <iostream>
#include <math.h>
#include "input.hpp" // Add this line if is_button_pressed is declared here, or declare the function below if not.

#define FOV (3.14159265358979323846 / 3.f)
#define PLAYER_SPEED (5.f)
#define MOUSE_SENSITIVITY (0.002f)
#define SCALE (50.f)

namespace silic {

engine::engine() {}

engine::~engine() {}

void engine::init(Input* in, wad_t* wad, const std::string& mapname, int width, int height) {
    glEnable(GL_DEPTH_TEST);
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
    
    float vertices[] = {
        1.f, 1.f, 0.f, //
        0.f, 1.f, 0.f,
        0.f, 0.f, 0.f,
        1.f, 0.f, 0.f
    };  

    uint32_t indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    mesh_create(&quad_mesh, 4, vertices, 6, indices, false);

    if (wad_read_map(mapname, &map, wad) != 0) {
        std::cerr << "Failed to read " << mapname << " from wad" << std::endl;
        return;
    }

    wall_node_t* tail = nullptr;
    for(int i = 0; i < map.num_linedefs; i++) {
        wall_node_t* node = new wall_node_t;
        node->next = nullptr;

        linedef_t* line = &map.linedefs[i];
        vec2_t start = map.vertices[line->start_idx];
        vec2_t end = map.vertices[line->end_idx];
        sector_t* sector = &map.sectors[map.sidedefs[line->front_sidedef].sector_idx];

        float floor = (float)sector->floor / SCALE;
        float ceiling = (float)sector->ceiling / SCALE;

        start.x/= SCALE;
        start.y/= SCALE;
        end.x/= SCALE;
        end.y/= SCALE;

        float dx = end.x - start.x;
        float dy = end.y - start.y;
        float length = sqrtf(dx * dx + dy * dy);
        float height = ceiling - floor;
        float angle = atan2f(dy, dx);

        mat4_t translation = mat4_translate((vec3_t){start.x, floor, start.y});
        mat4_t scale = mat4_scale((vec3_t){length, height, 1.f});
        mat4_t rotation = mat4_rotate((vec3_t){0.f, 1.f, 0.f}, angle);
        node->model = mat4_mul(mat4_mul(scale, rotation), translation);
        node->sector = sector;
        if (!wall_list) {
            wall_list = node;
        } else {
            tail->next = node;
        }
        tail = node;
    }
}
void engine::update(float delta) {
    float speed = input->is_button_pressed(button::KEY_LSHIFT) ? PLAYER_SPEED * 2.f : PLAYER_SPEED;
    camera_update_direction_vectors(&camera);
    if(input->is_button_pressed(button::KEY_W)) { // forward
        camera.position = vec3_add(camera.position, vec3_scale(camera.forward, speed * delta));
    }

    if(input->is_button_pressed(button::KEY_S)) { // backward
        camera.position = vec3_add(camera.position, vec3_scale(camera.forward, -speed * delta));
    }

    if(input->is_button_pressed(button::KEY_D)) { //right
        camera.position = vec3_add(camera.position, vec3_scale(camera.right, -speed * delta));
    }

    if(input->is_button_pressed(button::KEY_A)) { //left
        camera.position = vec3_add(camera.position, vec3_scale(camera.right, speed * delta));
    }

    if(input->is_button_pressed(button::KEY_SPACE)) { // up
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
        camera.yaw += dx * MOUSE_SENSITIVITY;
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
    for(wall_node_t* node = wall_list; node != nullptr; node = node->next) {
        vec3_t color = get_random_color((void*)node->sector);
        m_renderer.renderer_draw_mesh(&quad_mesh, node->model, (vec4_t){color.x, color.y, color.z, 1.f});
    }
}

vec3_t engine::get_random_color(const void *seed){
    srand((uintptr_t)seed);
    return (vec3_t){
        (float)(rand() % 1000) / 1000.f,
        (float)(rand() % 1000) / 1000.f,
        (float)(rand() % 1000) / 1000.f
    };
}

} // namespace silic
