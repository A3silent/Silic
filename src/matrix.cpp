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

mat4_t mat4_rotate(const vec3_t axis, float angle) {
    float c = cosf(angle);
    float s = sinf(angle);
    float t = 1.0f - c;

    vec3_t axis_normalized = vec3_normalize(axis);
    float x = axis_normalized.x, y = axis_normalized.y, z = axis_normalized.z;

    mat4_t result;
    result.a1 = t * x * x + c;
    result.a2 = t * x * y - s * z;
    result.a3 = t * x * z + s * y;
    result.a4 = 0.0f;

    result.b1 = t * x * y + s * z;
    result.b2 = t * y * y + c;
    result.b3 = t * y * z - s * x;
    result.b4 = 0.0f;

    result.c1 = t * x * z - s * y;
    result.c2 = t * y * z + s * x;
    result.c3 = t * z * z + c;
    result.c4 = 0.0f;

    result.d1 = 0.0f;
    result.d2 = 0.0f;
    result.d3 = 0.0f;
    result.d4 = 1.0f;

    return result;
}
