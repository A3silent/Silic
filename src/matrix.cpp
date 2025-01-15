#include "matrix.hpp"

#include <math.h>

mat4_t mat4_identity() {
  mat4_t mat = {0};

  mat.a1 = 1.f;
  mat.b2 = 1.f;
  mat.c3 = 1.f;
  mat.d4 = 1.f;

  return mat;
}

mat4_t mat4_mul(mat4_t m1, mat4_t m2) {
  mat4_t result;

  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result.m[i][j] = m1.m[i][0] * m2.m[0][j] + m1.m[i][1] * m2.m[1][j] +
                       m1.m[i][2] * m2.m[2][j] + m1.m[i][3] * m2.m[3][j];
    }
  }

  return result;
}

mat4_t mat4_translate(vec3_t translation) {
  mat4_t mat = mat4_identity();

  mat.d1 = translation.x;
  mat.d2 = translation.y;
  mat.d3 = translation.z;

  return mat;
}

mat4_t mat4_scale(vec3_t scale) {
  mat4_t mat = {0};

  mat.a1 = scale.x;
  mat.b2 = scale.y;
  mat.c3 = scale.z;
  mat.d4 = 1.f;

  return mat;
}

mat4_t mat4_perspective(float fov, float aspect_ratio, float near, float far) {
  mat4_t result = {0};

  float tan_half_fov = tanf(fov / 2.0f);
  float range_inv    = 1.0f / (near - far);

  result.a1 = 1.0f / (aspect_ratio * tan_half_fov);
  result.b2 = 1.0f / tan_half_fov;
  result.c3 = (near + far) * range_inv;
  result.c4 = -1.0f;
  result.d3 = 2.0f * near * far * range_inv;
  result.d4 = 0.0f;

  return result;
}

mat4_t mat4_ortho(float left, float right, float bottom, float top, float near,
                  float far) {
  mat4_t mat = {0};

  mat.a1 = 2.f / (right - left);
  mat.b2 = 2.f / (top - bottom);
  mat.c3 = -2.f / (far - near);
  mat.d1 = -(right + left) / (right - left);
  mat.d2 = -(top + bottom) / (top - bottom);
  mat.d3 = -(far + near) / (far - near);
  mat.d4 = 1.f;

  return mat;
}
