#include <math.h>

#include "globals.h"
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
#include "projection.h"
#include "f_texture.h"
#include "light.h"

void draw_mesh(
    PixelBuffer *pb,
    FTexture *z_buffer,

    State *state,
    PixelBuffer *texture,
    SFA *vertices,
    SU32A *indices,
    SFA *texcoords,
    SU32A *texcoord_indices,
    SU32A *colors,

    Vec3 pos,
    Vec3 rot,
    Vec3 scale)
{

    Mat4 model = mat4_create_model(pos, rot, scale);
    SFA *model_transformed_vertices = sfa_transform_vertices(vertices, &model);
    // calculate normals
    // vertices are still in x y z x y z x y z
    int num_faces = indices->length / 3;
    SFA *normals = sfa_new(num_faces * 3);
    for (int face = 0; face < num_faces; face++)
    {
        int idx1 = indices->data[face * 3];
        int idx2 = indices->data[face * 3 + 1];
        int idx3 = indices->data[face * 3 + 2];

        Vec3 p1 = vec3_create(
            model_transformed_vertices->data[idx1 * 4],
            model_transformed_vertices->data[idx1 * 4 + 1],
            model_transformed_vertices->data[idx1 * 4 + 2]);
        Vec3 p2 = vec3_create(
            model_transformed_vertices->data[idx2 * 4],
            model_transformed_vertices->data[idx2 * 4 + 1],
            model_transformed_vertices->data[idx2 * 4 + 2]);
        Vec3 p3 = vec3_create(
            model_transformed_vertices->data[idx3 * 4],
            model_transformed_vertices->data[idx3 * 4 + 1],
            model_transformed_vertices->data[idx3 * 4 + 2]);

        Vec3 normal = vec3_cross(vec3_sub(p2, p1), vec3_sub(p3, p1));
        normal = vec3_normalize(normal);

        normals->data[face] = normal.x;
        normals->data[face + 1] = normal.y;
        normals->data[face + 2] = normal.z;
    }

    Mat4 vp = mat4_create_vp(
        state->camera_pos, state->camera_target, state->camera_up,
        degrees_to_radians(90.0), (float)pb->width / (float)pb->height, 0.1f, 100.0f);

    Mat4 mvp = mat4_multiply(vp, model);

    SFA *transformed_vertices // x y z w
        = sfa_transform_vertices(vertices, &mvp);

    perspective_divide(transformed_vertices); // Implement perspective_divide

    // Map to screen coordinates with camera space distance
    SFA *screen_coords // x y depth
        = sfa_new(transformed_vertices->length / 4 * 3);
    map_to_screen_keep_z(transformed_vertices, screen_coords, pb->width, pb->height);

    // Render lines
    // draw_tris_lines_with_depth(pb, screen_coords, assets->gba_mesh->vertex_indices, COLOR_WHITE);
    // draw_tris_with_colors_and_face_numbers(
    //     pb, assets->charmap_white,
    //     screen_coords, assets->gba_mesh->vertex_indices, assets->gba_mesh->colors, 1.0, COLOR_BLACK);
    // draw_tris_with_colors(pb, screen_coords, assets->gba_mesh->vertex_indices, assets->gba_mesh->colors);
    // draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, assets->gba_mesh->vertex_indices, assets->gba_mesh->colors);
    // use the colors calculated from the lighting
    // draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, indices, colors);
    // draw_tris_with_colors_and_depth_with_face_buffer(pb, z_buffer, face_buffer, screen_coords, indices, colors);

    // calculate cam dir
    Vec3 cam_dir = vec3_sub(state->camera_target, state->camera_pos);
    cam_dir = vec3_normalize(cam_dir);

    draw_tris_textured(
        pb,
        texture,
        z_buffer,
        screen_coords,
        indices,
        texcoords,
        texcoord_indices,
        normals,
        cam_dir);

    // Cleanup
    sfa_free(model_transformed_vertices);
    sfa_free(transformed_vertices);
    sfa_free(screen_coords);
    sfa_free(normals);
}

void draw(PixelBuffer *pb, FTexture *z_buffer, State *state, Assets *assets)
{
    // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);
    draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);

    // every 10 frames increment earth_mfpb
    if (state->frame_count % 4 == 0)
    {
        mfpb_next_frame(assets->earth_mfpb);
    }

    float scalef = 20.0;
    Mesh *mesh = assets->gba_mesh;
    PixelBuffer *texture = assets->gba_texture;
    // Mesh *mesh = assets->earth_mesh;
    // PixelBuffer *texture = assets->manhat_texture;
    // PixelBuffer *texture = assets->earth_mfpb->frames[assets->earth_mfpb->current_frame];

    float x_pos = 0.0;
    float y_pos = 0.0;
    float z_pos = 0.0;
    float x_angle = 0.0;
    float y_angle = state->frame_count * 0.01 - 0.3;
    // float y_angle = radians_to_degrees(90.0);
    float z_angle = 0.0;
    draw_mesh(
        pb,
        z_buffer,

        state,
        texture,
        mesh->vertices,
        mesh->vertex_indices,
        mesh->texcoords,
        mesh->texcoord_indices,
        mesh->colors,

        vec3_create(x_pos, y_pos, z_pos),       // position
        vec3_create(x_angle, y_angle, z_angle), // rotation
        vec3_create(scalef, scalef, scalef)     // scale
    );

    // Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
    // draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFFFFFFFF);

    // tl, 21/93
    // br, 47/110

    // we want a copy of last frame, flipped vertically and horizontally
    // then we want to blit that to the gba_texture
    // at the coords
    // tl: 21, 93
    // br: 47, 110

    // we can do this with coordinate mapping
    // int y_start = 93;
    // int y_end = 110;
    // int x_start = 21;
    // int x_end = 47;
    int y_start = 373;
    int y_end = 442;
    int x_start = 88;
    int x_end = 189;

    for (int y = y_start; y < y_end + 1; y++)
    {
        for (int x = x_start; x < x_end + 1; x++)
        {
            // get the sample position for last_frame
            // float map_range(float value, float in_min, float in_max, float out_min, float out_max);
            int sample_x = (int)map_range(x, x_start, x_end, 0, pb->width - 1);
            int sample_y = (int)map_range(y, y_start, y_end, pb->height - 1, 0);
            uint32_t color = pixel_buffer_get(pb, sample_x, sample_y);
            // replace clear with black
            if (color == 0x00000000)
            {
                color = 0x000000FF;
            }
            pixel_buffer_set(texture, x, y, color);
        }
    }
}
