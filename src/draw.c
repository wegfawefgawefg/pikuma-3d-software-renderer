#include <math.h>

#include "draw.h"
#include "draw_lib.h"
#include "primitives.h"
#include "input.h"
#include "colors.h"
#include "pixel_buffer.h"
#include "assets.h"
#include "utils.h"
#include "vec2.h"
#include "vec3.h"
#include "sfa.h"
#include "mesh.h"
#include "mat4.h"

// for reference
// // Define a 4x4 matrix as a 2D array
// typedef struct
// {
//     float m[4][4];
// } Mat4;

// // Matrix creation and utility functions
// Mat4 mat4_identity(void);
// Mat4 mat4_create(
//     float m00, float m01, float m02, float m03,
//     float m10, float m11, float m12, float m13,
//     float m20, float m21, float m22, float m23,
//     float m30, float m31, float m32, float m33);
// Mat4 mat4_copy(Mat4 src);

// // Matrix operations
// Mat4 mat4_multiply(Mat4 a, Mat4 b);
// Vec3 mat4_multiply_vec3(Mat4 m, Vec3 v); // Assumes w = 1
// Mat4 mat4_translate(Vec3 v);
// Mat4 mat4_scale(Vec3 v);
// Mat4 mat4_rotate_x(float angle_rad);
// Mat4 mat4_rotate_y(float angle_rad);
// Mat4 mat4_rotate_z(float angle_rad);
// Mat4 mat4_rotate(float angle_rad, Vec3 axis);
// Mat4 mat4_perspective(float fov_rad, float aspect, float near, float far);
// Mat4 mat4_look_at(Vec3 eye, Vec3 center, Vec3 up);
// Mat4 mat4_inverse(Mat4 m);
// Mat4 mat4_transpose(Mat4 m);

void draw(PixelBuffer *pb, State *state, Assets *assets)
{
    {
        draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);

        Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
        draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFFFFFFFF);

        // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);

        // draw the gba mesh
        // rotate based on time
        // translate based on camera position
        {
            float scalef = 20.0;
            float time = SDL_GetTicks() / 1000.0f;
            float angle = time * 0.5f;
            Mat4 rotation = mat4_rotate(angle, vec3_create(0.0f, 1.0f, 0.0f));
            Mat4 translation = mat4_translate(vec3_create(state->camera_pos.x, state->camera_pos.y, state->camera_pos.z));
            Mat4 scale = mat4_scale(vec3_create(scalef, scalef, scalef));
            Mat4 model = mat4_multiply(translation, rotation);
            model = mat4_multiply(model, scale);

            Mesh *gba_mesh = mesh_copy(assets->gba_mesh);
            mesh_transform(gba_mesh, model);
            SFA *projected_mesh = sfa_isometric_project(gba_mesh->vertices);
            // draw_tris(pb, projected_mesh, gba_mesh->indices, 0xFFFFFFFF);
            // draw_points(pb, projected_mesh, 0xFF0000FF);
            draw_tris_lines(pb, projected_mesh, gba_mesh->indices, 0xFFFFFFFF);

            sfa_free(projected_mesh);
            mesh_free(gba_mesh);
        }
    }
}