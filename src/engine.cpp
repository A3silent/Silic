#include "engine.hpp"
#include "matrix.hpp"
#include "palette.hpp"
#include <iostream>
#include <math.h>
#include "input.hpp" // Add this line if is_button_pressed is declared here, or declare the function below if not.

#define FOV (3.14159265358979323846 / 3.f)
#define PI (3.14159265358979323846)
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
        .yaw = (PI / 2.f),
        .pitch = 0.f,
    };

    size = m_renderer.renderer_get_size();
    mat4_t projection = mat4_perspective(FOV, size.x/size.y, .1f, 1000.f);
    m_renderer.renderer_set_projection(projection);
    
    vertex_t vertices[] = {
        {.position = {1.f, 1.f, 0.f}}, // top-right
        {.position = {0.f, 1.f, 0.f}}, // bottom-right
        {.position = {0.f, 0.f, 0.f}}, // bottom-left
        {.position = {1.0, 0.f, 0.f}}, // top-left
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

    generate_meshes(&map, &gl_map);
    wad_read_playpal(&palette, wad);
    GLuint palette_texture = color_create_texture(&palette);
    m_renderer.renderer_set_palette_texture(palette_texture);

    flat_texture_t* flat_textures = wad_read_flat_texture(&num_flats, wad);
    //std::cout << "Number of flats: " << num_flats << std::endl;
    flat_texture = new GLuint[num_flats];
    for (int i = 0; i < num_flats; i++) {
        flat_texture[i] = generate_flat_texture(&flat_textures[i]);
    }
}

void engine::generate_meshes(const map_t *map, const gl_map_t* gl_map){

    flat_node_t* tail_flat = nullptr;
    for(int i = 0; i < gl_map->num_subsectors; i++) {
        flat_node_t* node = new flat_node_t;
        node ->next = nullptr;
        node ->sector = nullptr;
        gl_subsector_t* subsector = &gl_map->subsectors[i];
        size_t n_vertices = subsector->num_segs;
        vertex_t* vertices = new vertex_t[n_vertices];

        for (int j = 0; j < subsector->num_segs; j++) {
            gl_segment_t* segment = &gl_map->segments[j + subsector->first_seg];

            if(node ->sector == nullptr && segment->linedef != 0xffff) {
                linedef_t* line = &map->linedefs[segment->linedef];
                int sec = -1;

                int sidedef_idx = (line->flags & LINEDEF_FLAG_TWO_SIDED)
                    ? (segment->side == 0 ? line->front_sidedef : line->back_sidedef)
                    : line->front_sidedef;
                sec = map->sidedefs[sidedef_idx].sector_idx;

                if (sec >= 0) { node->sector = &map->sectors[sec]; }
            }

            vec2_t v;
            if(segment->start_vertex & VERT_IS_GL) {
                v = gl_map->vertices[segment->start_vertex & 0x7fff];
            }else{
                v = map->vertices[segment->start_vertex];
            }

            vertices[j] = (vertex_t){
                .position = {v.x / SCALE, 0.f, -v.y / SCALE},
                .texcoord = {v.x / FLAT_TEXTURE_SIZE, -v.y / FLAT_TEXTURE_SIZE}
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
    for(int i = 0; i < map->num_linedefs; i++) {
            //floor
        linedef_t* line = &map->linedefs[i];
        if(line->flags & LINEDEF_FLAG_TWO_SIDED) {
            wall_node_t* floor_node = new wall_node_t;
            floor_node->next = nullptr;
            vec2_t start = map->vertices[line->start_idx];
            vec2_t end = map->vertices[line->end_idx];
            sidedef_t* front_sidedef = &map->sidedefs[line->front_sidedef];
            sector_t* front_sector = &map->sectors[front_sidedef->sector_idx];

            sidedef_t* back_sidedef = &map->sidedefs[line->back_sidedef];
            sector_t* back_sector = &map->sectors[back_sidedef->sector_idx];

            vec3_t f0 = {start.x, (float)front_sector->floor, -start.y};
            vec3_t f1 = {end.x, (float)front_sector->floor, -end.y};
            vec3_t f2 = {end.x, (float)back_sector->floor, -end.y};
            vec3_t f3 = {start.x, (float)back_sector->floor, -start.y};

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

            vec3_t c0 = {start.x, (float)front_sector->ceiling, -start.y};
            vec3_t c1 = {end.x, (float)front_sector->ceiling, -end.y};
            vec3_t c2 = {end.x, (float)back_sector->ceiling, -end.y};
            vec3_t c3 = {start.x, (float)back_sector->ceiling, -start.y};

            ceiling_node->model = model_from_vertices(c0, c1, c2, c3);
            ceiling_node->sector = front_sector;

            tail->next = ceiling_node;
            tail = ceiling_node;
        }else{
            wall_node_t* node = new wall_node_t;
            node->next = nullptr;
            vec2_t start = map->vertices[line->start_idx];
            vec2_t end = map->vertices[line->end_idx];
            sidedef_t* sidedef = &map->sidedefs[line->front_sidedef];
            sector_t* sector = &map->sectors[sidedef->sector_idx];

            vec3_t f0 = {start.x, (float)sector->floor, -start.y};
            vec3_t f1 = {end.x, (float)sector->floor, -end.y};
            vec3_t f2 = {end.x, (float)sector->ceiling, -end.y};
            vec3_t f3 = {start.x, (float)sector->ceiling, -start.y};

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
    mat4_t view = mat4_look_at(camera.position, vec3_add(camera.position, camera.forward), camera.up);
    m_renderer.renderer_set_viewport(view);

    m_renderer.renderer_set_draw_texture(0);
    for(wall_node_t* node = wall_list; node != nullptr; node = node->next) {
        
        srand((uintptr_t)node->sector);
        int color = rand() % NUM_COLORS;
        m_renderer.renderer_draw_mesh(&quad_mesh, node->model, color);
    }
    //floor
    for(flat_node_t* node = flat_list; node != nullptr; node = node->next) {
        int floor_index =   node->sector->floor_tex < 0 || node->sector->floor_tex >= num_flats
                                    ? 0
                                    : flat_texture[node->sector->floor_tex];
        int ceiling_index = node->sector->ceiling_tex < 0 || node->sector->ceiling_tex >= num_flats
                                    ? 0
                                    : flat_texture[node->sector->ceiling_tex];

        m_renderer.renderer_set_draw_texture(floor_index);
        m_renderer.renderer_draw_mesh(
            &node->mesh,
            mat4_translate((vec3_t){0.f, node->sector->floor / SCALE, 0.f}),
            0);

        m_renderer.renderer_set_draw_texture(ceiling_index);
        m_renderer.renderer_draw_mesh(&node->mesh, 
            mat4_translate((vec3_t){0.f, node->sector->ceiling / SCALE, 0.f}),
            0);
    }

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
