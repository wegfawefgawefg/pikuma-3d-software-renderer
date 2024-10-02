#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"

// Define a 4x4 matrix as a 2D array
typedef struct
{
    float m[4][4];
} Mat4;

// Matrix creation and utility functions
Mat4 mat4_identity(void);
Mat4 mat4_create(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33);
Mat4 mat4_copy(Mat4 src);

// Matrix operations
Mat4 mat4_multiply(Mat4 a, Mat4 b);
Vec3 mat4_multiply_vec3(Mat4 m, Vec3 v); // Assumes w = 1
Mat4 mat4_translate(Vec3 v);
Mat4 mat4_scale(Vec3 v);
Mat4 mat4_rotate_x(float angle_rad);
Mat4 mat4_rotate_y(float angle_rad);
Mat4 mat4_rotate_z(float angle_rad);
Mat4 mat4_rotate(float angle_rad, Vec3 axis);
Mat4 mat4_perspective(float fov_rad, float aspect, float near, float far);
Mat4 mat4_look_at(Vec3 eye, Vec3 center, Vec3 up);
Mat4 mat4_inverse(Mat4 m);
Mat4 mat4_transpose(Mat4 m);

// Optional: Functions for printing matrices (useful for debugging)
void mat4_print(Mat4 m);

#endif // MAT4_H
