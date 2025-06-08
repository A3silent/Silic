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
    std::string gl_mapname = "GL_" + mapname;
    if (wad_read_gl_map(gl_mapname, &gl_map, wad) != 0) {
        std::cerr << "Failed to read GL map " << mapname << " from wad" << std::endl;
        return;
    }

    // for (int i = 0; i < gl_map.num_vertices; i++) {
    //     std::cout << "Vertex " << i << ": (" 
    //               << gl_map.vertices[i].x << ", " 
    //               << gl_map.vertices[i].y << ")" << std::endl;
    // }

    flat_node_t* tail_flat = nullptr;
    for(int i = 0; i < gl_map.num_subsectors; i++) {
        flat_node_t* node = new flat_node_t;
        node ->next = nullptr;
        node ->sector = nullptr;
        gl_subsector_t* subsector = &gl_map.subsectors[i];
        size_t n_vertices = subsector->num_segs;
        vertex_t* vertices = new vertex_t[n_vertices];

        for (int j = 0; j < subsector->num_segs; j++) {
            gl_segment_t* segment = &gl_map.segments[j + subsector->first_seg];

            if(node ->sector == nullptr && segment->linedef != 0xffff) {
                linedef_t* line = &map.linedefs[segment->linedef];
                int sec = -1;

                if (line -> flags & LINEDEF_FLAG_TWO_SIDED) {
                    if(segment->side == 0) {
                        sec = map.sidedefs[line->front_sidedef].sector_idx;
                    } else {
                        sec = map.sidedefs[line->back_sidedef].sector_idx;
                    }
                }else{
                sec = map.sidedefs[line->front_sidedef].sector_idx;
                }
                if (sec >= 0) { node->sector = &map.sectors[sec]; }
            }

            vec2_t v;
            if(segment->start_vertex & VERT_IS_GL) {
                v = gl_map.vertices[segment->start_vertex & 0x7fff];
            }else{
                v = map.vertices[segment->start_vertex];
            }

            vertices[j] = (vertex_t){
                .position = {v.x / 50.f, 0.f, v.y / 50.f},
            };
        }

        size_t num_indices = 3 * (n_vertices - 2);
        uint32_t *indices = new uint32_t[num_indices];
        for (int j = 0, k = 1; j < num_indices; j += 3, k++) {
        indices[j]     = 0;
        indices[j + 1] = k;
        indices[j + 2] = k + 1;
        }

        mesh_create(&node->mesh, n_vertices, vertices, num_indices, indices, false);

        if(!flat_list) {
            flat_list = node;
        } else {
            tail_flat->next = node;
        }
        tail_flat = node;
    }


    wall_node_t* tail = nullptr;
    for(int i = 0; i < map.num_linedefs; i++) {
            //floor
        linedef_t* line = &map.linedefs[i];
        if(line->flags & LINEDEF_FLAG_TWO_SIDED) {
            wall_node_t* floor_node = new wall_node_t;
            floor_node->next = nullptr;
            vec2_t start = map.vertices[line->start_idx];
            vec2_t end = map.vertices[line->end_idx];
            sidedef_t* front_sidedef = &map.sidedefs[line->front_sidedef];
            sector_t* front_sector = &map.sectors[front_sidedef->sector_idx];

            sidedef_t* back_sidedef = &map.sidedefs[line->back_sidedef];
            sector_t* back_sector = &map.sectors[back_sidedef->sector_idx];

            vec3_t f0 = {start.x, (float)front_sector->floor, start.y};
            vec3_t f1 = {end.x, (float)front_sector->floor, end.y};
            vec3_t f2 = {end.x, (float)back_sector->floor, end.y};
            vec3_t f3 = {start.x, (float)back_sector->floor, start.y};

            floor_node->model = model_from_vertices(f0, f1, f2, f3);
            floor_node->sector = front_sector;

            if (!wall_list) {
                wall_list = floor_node;
            } else {
                tail->next = floor_node;
            }
            tail = floor_node;
            
            //ceiling
            wall_node_t* ceiling_node = new wall_node_t;
            ceiling_node->next = nullptr;

            vec3_t c0 = {start.x, (float)front_sector->ceiling, start.y};
            vec3_t c1 = {end.x, (float)front_sector->ceiling, end.y};
            vec3_t c2 = {end.x, (float)back_sector->ceiling, end.y};
            vec3_t c3 = {start.x, (float)back_sector->ceiling, start.y};

            ceiling_node->model = model_from_vertices(c0, c1, c2, c3);
            ceiling_node->sector = front_sector;

            tail->next = ceiling_node;
            tail = ceiling_node;
        }else{
            wall_node_t* node = new wall_node_t;
            node->next = nullptr;
            vec2_t start = map.vertices[line->start_idx];
            vec2_t end = map.vertices[line->end_idx];
            sidedef_t* sidedef = &map.sidedefs[line->front_sidedef];
            sector_t* sector = &map.sectors[sidedef->sector_idx];

            vec3_t f0 = {start.x, (float)sector->floor, start.y};
            vec3_t f1 = {end.x, (float)sector->floor, end.y};
            vec3_t f2 = {end.x, (float)sector->ceiling, end.y};
            vec3_t f3 = {start.x, (float)sector->ceiling, start.y};

            node->model = model_from_vertices(f0, f1, f2, f3);
            node->sector = sector;

            if (!wall_list) {
                wall_list = node;
            } else {
                tail->next = node;   
            }
            tail = node;
        }
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
    m_renderer.renderer_clear();
    mat4_t view = mat4_look_at(camera.position, vec3_add(camera.position, camera.forward), camera.up);
    m_renderer.renderer_set_viewport(view);
    for(wall_node_t* node = wall_list; node != nullptr; node = node->next) {
        vec3_t color = get_random_color((void*)node->sector);
        m_renderer.renderer_draw_mesh(&quad_mesh, node->model, (vec4_t){color.x, color.y, color.z, 1.f});
    }
    //floor
    for(flat_node_t* node = flat_list; node != nullptr; node = node->next) {
        vec3_t color = vec3_scale(get_random_color((void*)node->sector), node->sector->light_level / 255.f * 0.7f);
        m_renderer.renderer_draw_mesh(
            &node->mesh,
            mat4_translate((vec3_t){0.f, node->sector->floor / SCALE, 0.f}),
            (vec4_t){color.x, color.y, color.z, 1.f});

        color = vec3_scale(get_random_color((void*)node->sector), node->sector->light_level / 255.f);
        m_renderer.renderer_draw_mesh(&node->mesh, 
            mat4_translate((vec3_t){0.f, node->sector->ceiling / SCALE, 0.f}),
            (vec4_t){color.x, color.y, color.z, 1.f});
    }

    //ceiling
    // for(flat_node_t* node = flat_list; node != nullptr; node = node->next) {
    //     
    // }

}

vec3_t engine::get_random_color(const void *seed){
    srand((uintptr_t)seed);
    return (vec3_t){
        (float)(rand() % 1000) / 1000.f,
        (float)(rand() % 1000) / 1000.f,
        (float)(rand() % 1000) / 1000.f
    };
}

mat4_t engine::model_from_vertices(vec3_t v0, vec3_t v1, vec3_t v2, vec3_t v3) {
    v0 = vec3_scale(v0, 1.f / SCALE);
    v1 = vec3_scale(v1, 1.f / SCALE);
    v2 = vec3_scale(v2, 1.f / SCALE);
    v3 = vec3_scale(v3, 1.f / SCALE);

    float dx = v1.x - v0.x;
    float dy = v1.z - v0.z;
    float length = sqrtf(dx * dx + dy * dy);
    float height = v3.y - v0.y;
    float angle = atan2f(dy, dx);

    mat4_t translation = mat4_translate(v0);
    mat4_t scale = mat4_scale((vec3_t){length, height, 1.f});
    mat4_t rotation = mat4_rotate((vec3_t){0.f, 1.f, 0.f}, angle);
    return mat4_mul(mat4_mul(scale, rotation), translation);
}

} // namespace silic
