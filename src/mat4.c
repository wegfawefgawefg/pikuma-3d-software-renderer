#include "mat4.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

// Utility function to set matrix to identity
Mat4 mat4_identity(void)
{
    Mat4 result = {0};
    for (int i = 0; i < 4; i++)
    {
        result.m[i][i] = 1.0f;
    }
    return result;
}

// Create a matrix with specified elements
Mat4 mat4_create(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33)
{
    Mat4 result = {
        .m = {
            {m00, m01, m02, m03},
            {m10, m11, m12, m13},
            {m20, m21, m22, m23},
            {m30, m31, m32, m33}}};
    return result;
}

// Copy a matrix
Mat4 mat4_copy(Mat4 src)
{
    Mat4 dest;
    memcpy(dest.m, src.m, sizeof(float) * 16);
    return dest;
}

// Matrix multiplication: result = a * b
Mat4 mat4_multiply(Mat4 a, Mat4 b)
{
    Mat4 result = mat4_identity();
    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
        {
            result.m[row][col] =
                a.m[row][0] * b.m[0][col] +
                a.m[row][1] * b.m[1][col] +
                a.m[row][2] * b.m[2][col] +
                a.m[row][3] * b.m[3][col];
        }
    }
    return result;
}

// Multiply a matrix with a Vec3 (assuming w = 1)
Vec3 mat4_multiply_vec3(Mat4 m, Vec3 v)
{
    float x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3];
    float y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3];
    float z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3];
    float w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3];

    if (w != 0.0f && w != 1.0f)
    {
        x /= w;
        y /= w;
        z /= w;
    }

    Vec3 result = {x, y, z};
    return result;
}

// Create a translation matrix
Mat4 mat4_translate(Vec3 v)
{
    Mat4 result = mat4_identity();
    result.m[0][3] = v.x;
    result.m[1][3] = v.y;
    result.m[2][3] = v.z;
    return result;
}

// Create a scaling matrix
Mat4 mat4_scale(Vec3 v)
{
    Mat4 result = mat4_identity();
    result.m[0][0] = v.x;
    result.m[1][1] = v.y;
    result.m[2][2] = v.z;
    return result;
}

// Create a rotation matrix around the X-axis
Mat4 mat4_rotate_x(float angle_rad)
{
    Mat4 result = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    result.m[1][1] = c;
    result.m[1][2] = -s;
    result.m[2][1] = s;
    result.m[2][2] = c;
    return result;
}

// Create a rotation matrix around the Y-axis
Mat4 mat4_rotate_y(float angle_rad)
{
    Mat4 result = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    result.m[0][0] = c;
    result.m[0][2] = s;
    result.m[2][0] = -s;
    result.m[2][2] = c;
    return result;
}

// Create a rotation matrix around the Z-axis
Mat4 mat4_rotate_z(float angle_rad)
{
    Mat4 result = mat4_identity();
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    result.m[0][0] = c;
    result.m[0][1] = -s;
    result.m[1][0] = s;
    result.m[1][1] = c;
    return result;
}

// Create a rotation matrix around an arbitrary axis
Mat4 mat4_rotate(float angle_rad, Vec3 axis)
{
    Vec3 norm = vec3_normalize(axis);
    float x = norm.x;
    float y = norm.y;
    float z = norm.z;
    float c = cosf(angle_rad);
    float s = sinf(angle_rad);
    float one_c = 1.0f - c;

    Mat4 result = mat4_identity();
    result.m[0][0] = c + x * x * one_c;
    result.m[0][1] = x * y * one_c - z * s;
    result.m[0][2] = x * z * one_c + y * s;

    result.m[1][0] = y * x * one_c + z * s;
    result.m[1][1] = c + y * y * one_c;
    result.m[1][2] = y * z * one_c - x * s;

    result.m[2][0] = z * x * one_c - y * s;
    result.m[2][1] = z * y * one_c + x * s;
    result.m[2][2] = c + z * z * one_c;

    return result;
}

// Create a perspective projection matrix
Mat4 mat4_perspective(float fov_rad, float aspect, float near, float far)
{
    Mat4 result = {0};
    float f = 1.0f / tanf(fov_rad / 2.0f);
    result.m[0][0] = f / aspect;
    result.m[1][1] = f;
    result.m[2][2] = (far + near) / (near - far);
    result.m[2][3] = (2.0f * far * near) / (near - far);
    result.m[3][2] = -1.0f;
    return result;
}

// Create a look-at view matrix
Mat4 mat4_look_at(Vec3 eye, Vec3 center, Vec3 up)
{
    Vec3 f = vec3_normalize(vec3_sub(center, eye));
    Vec3 s = vec3_normalize(vec3_cross(f, up));
    Vec3 u = vec3_cross(s, f);

    Mat4 result = mat4_identity();
    result.m[0][0] = s.x;
    result.m[0][1] = s.y;
    result.m[0][2] = s.z;

    result.m[1][0] = u.x;
    result.m[1][1] = u.y;
    result.m[1][2] = u.z;

    result.m[2][0] = -f.x;
    result.m[2][1] = -f.y;
    result.m[2][2] = -f.z;

    result.m[0][3] = -vec3_dot(s, eye);
    result.m[1][3] = -vec3_dot(u, eye);
    result.m[2][3] = vec3_dot(f, eye);

    return result;
}

// Transpose of a matrix
Mat4 mat4_transpose(Mat4 m)
{
    Mat4 result;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            result.m[i][j] = m.m[j][i];
        }
    }
    return result;
}

// Inverse of a matrix (assuming it's invertible)
Mat4 mat4_inverse(Mat4 m)
{
    Mat4 inv;
    float det;
    int i;

    inv.m[0][0] = m.m[1][1] * m.m[2][2] * m.m[3][3] -
                  m.m[1][1] * m.m[2][3] * m.m[3][2] -
                  m.m[2][1] * m.m[1][2] * m.m[3][3] +
                  m.m[2][1] * m.m[1][3] * m.m[3][2] +
                  m.m[3][1] * m.m[1][2] * m.m[2][3] -
                  m.m[3][1] * m.m[1][3] * m.m[2][2];

    inv.m[0][1] = -m.m[0][1] * m.m[2][2] * m.m[3][3] +
                  m.m[0][1] * m.m[2][3] * m.m[3][2] +
                  m.m[2][1] * m.m[0][2] * m.m[3][3] -
                  m.m[2][1] * m.m[0][3] * m.m[3][2] -
                  m.m[3][1] * m.m[0][2] * m.m[2][3] +
                  m.m[3][1] * m.m[0][3] * m.m[2][2];

    inv.m[0][2] = m.m[0][1] * m.m[1][2] * m.m[3][3] -
                  m.m[0][1] * m.m[1][3] * m.m[3][2] -
                  m.m[1][1] * m.m[0][2] * m.m[3][3] +
                  m.m[1][1] * m.m[0][3] * m.m[3][2] +
                  m.m[3][1] * m.m[0][2] * m.m[1][3] -
                  m.m[3][1] * m.m[0][3] * m.m[1][2];

    inv.m[0][3] = -m.m[0][1] * m.m[1][2] * m.m[2][3] +
                  m.m[0][1] * m.m[1][3] * m.m[2][2] +
                  m.m[1][1] * m.m[0][2] * m.m[2][3] -
                  m.m[1][1] * m.m[0][3] * m.m[2][2] -
                  m.m[2][1] * m.m[0][2] * m.m[1][3] +
                  m.m[2][1] * m.m[0][3] * m.m[1][2];

    inv.m[1][0] = -m.m[1][0] * m.m[2][2] * m.m[3][3] +
                  m.m[1][0] * m.m[2][3] * m.m[3][2] +
                  m.m[2][0] * m.m[1][2] * m.m[3][3] -
                  m.m[2][0] * m.m[1][3] * m.m[3][2] -
                  m.m[3][0] * m.m[1][2] * m.m[2][3] +
                  m.m[3][0] * m.m[1][3] * m.m[2][2];

    inv.m[1][1] = m.m[0][0] * m.m[2][2] * m.m[3][3] -
                  m.m[0][0] * m.m[2][3] * m.m[3][2] -
                  m.m[2][0] * m.m[0][2] * m.m[3][3] +
                  m.m[2][0] * m.m[0][3] * m.m[3][2] +
                  m.m[3][0] * m.m[0][2] * m.m[2][3] -
                  m.m[3][0] * m.m[0][3] * m.m[2][2];

    inv.m[1][2] = -m.m[0][0] * m.m[1][2] * m.m[3][3] +
                  m.m[0][0] * m.m[1][3] * m.m[3][2] +
                  m.m[1][0] * m.m[0][2] * m.m[3][3] -
                  m.m[1][0] * m.m[0][3] * m.m[3][2] -
                  m.m[3][0] * m.m[0][2] * m.m[1][3] +
                  m.m[3][0] * m.m[0][3] * m.m[1][2];

    inv.m[1][3] = m.m[0][0] * m.m[1][2] * m.m[2][3] -
                  m.m[0][0] * m.m[1][3] * m.m[2][2] -
                  m.m[1][0] * m.m[0][2] * m.m[2][3] +
                  m.m[1][0] * m.m[0][3] * m.m[2][2] +
                  m.m[2][0] * m.m[0][2] * m.m[1][3] -
                  m.m[2][0] * m.m[0][3] * m.m[1][2];

    inv.m[2][0] = m.m[1][0] * m.m[2][1] * m.m[3][3] -
                  m.m[1][0] * m.m[2][3] * m.m[3][1] -
                  m.m[2][0] * m.m[1][1] * m.m[3][3] +
                  m.m[2][0] * m.m[1][3] * m.m[3][1] +
                  m.m[3][0] * m.m[1][1] * m.m[2][3] -
                  m.m[3][0] * m.m[1][3] * m.m[2][1];

    inv.m[2][1] = -m.m[0][0] * m.m[2][1] * m.m[3][3] +
                  m.m[0][0] * m.m[2][3] * m.m[3][1] +
                  m.m[2][0] * m.m[0][1] * m.m[3][3] -
                  m.m[2][0] * m.m[0][3] * m.m[3][1] -
                  m.m[3][0] * m.m[0][1] * m.m[2][3] +
                  m.m[3][0] * m.m[0][3] * m.m[2][1];

    inv.m[2][2] = m.m[0][0] * m.m[1][1] * m.m[3][3] -
                  m.m[0][0] * m.m[1][3] * m.m[3][1] -
                  m.m[1][0] * m.m[0][1] * m.m[3][3] +
                  m.m[1][0] * m.m[0][3] * m.m[3][1] +
                  m.m[3][0] * m.m[0][1] * m.m[1][3] -
                  m.m[3][0] * m.m[0][3] * m.m[1][1];

    inv.m[2][3] = -m.m[0][0] * m.m[1][1] * m.m[2][3] +
                  m.m[0][0] * m.m[1][3] * m.m[2][1] +
                  m.m[1][0] * m.m[0][1] * m.m[2][3] -
                  m.m[1][0] * m.m[0][3] * m.m[2][1] -
                  m.m[2][0] * m.m[0][1] * m.m[1][3] +
                  m.m[2][0] * m.m[0][3] * m.m[1][1];

    inv.m[3][0] = -m.m[1][0] * m.m[2][1] * m.m[3][2] +
                  m.m[1][0] * m.m[2][2] * m.m[3][1] +
                  m.m[2][0] * m.m[1][1] * m.m[3][2] -
                  m.m[2][0] * m.m[1][2] * m.m[3][1] -
                  m.m[3][0] * m.m[1][1] * m.m[2][2] +
                  m.m[3][0] * m.m[1][2] * m.m[2][1];

    inv.m[3][1] = m.m[0][0] * m.m[2][1] * m.m[3][2] -
                  m.m[0][0] * m.m[2][2] * m.m[3][1] -
                  m.m[2][0] * m.m[0][1] * m.m[3][2] +
                  m.m[2][0] * m.m[0][2] * m.m[3][1] +
                  m.m[3][0] * m.m[0][1] * m.m[2][2] -
                  m.m[3][0] * m.m[0][2] * m.m[2][1];

    inv.m[3][2] = -m.m[0][0] * m.m[1][1] * m.m[3][2] +
                  m.m[0][0] * m.m[1][2] * m.m[3][1] +
                  m.m[1][0] * m.m[0][1] * m.m[3][2] -
                  m.m[1][0] * m.m[0][2] * m.m[3][1] -
                  m.m[3][0] * m.m[0][1] * m.m[1][2] +
                  m.m[3][0] * m.m[0][2] * m.m[1][1];

    inv.m[3][3] = m.m[0][0] * m.m[1][1] * m.m[2][2] -
                  m.m[0][0] * m.m[1][2] * m.m[2][1] -
                  m.m[1][0] * m.m[0][1] * m.m[2][2] +
                  m.m[1][0] * m.m[0][2] * m.m[2][1] +
                  m.m[2][0] * m.m[0][1] * m.m[1][2] -
                  m.m[2][0] * m.m[0][2] * m.m[1][1];

    det = m.m[0][0] * inv.m[0][0] + m.m[0][1] * inv.m[1][0] + m.m[0][2] * inv.m[2][0] + m.m[0][3] * inv.m[3][0];

    if (det == 0)
        return mat4_identity(); // Return identity as a fallback

    det = 1.0f / det;

    for (i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            inv.m[i][j] *= det;

    return inv;
}

// Optional: Print matrix for debugging
void mat4_print(Mat4 m)
{
    for (int i = 0; i < 4; i++)
    {
        printf("| ");
        for (int j = 0; j < 4; j++)
        {
            printf("%8.3f ", m.m[i][j]);
        }
        printf("|\n");
    }
}

// Example of rotation around arbitrary axis combined with translation
Mat4 mat4_rotate_translate(float angle_rad, Vec3 axis, Vec3 translation)
{
    Mat4 rotation = mat4_rotate(angle_rad, axis);
    Mat4 translation_matrix = mat4_translate(translation);
    return mat4_multiply(translation_matrix, rotation);
}
