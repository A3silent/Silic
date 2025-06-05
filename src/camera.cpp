#include "camera.hpp"
#include "vector.hpp"
#include <math.h>

namespace silic {
    void camera_update_direction_vectors(camera_t *camera){
        camera->forward = (vec3_t){
        cosf(camera->yaw) * cosf(camera->pitch),
        sinf(camera->pitch),
        sinf(camera->yaw) * cosf(camera->pitch),
    };

    camera->right = vec3_normalize(vec3_cross(WORLD_UP, camera->forward));
    camera->up    = vec3_normalize(vec3_cross(camera->forward, camera->right));
    }
}