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

// draw the gba mesh
void draw_gba_mesh(PixelBuffer *pb, FTexture *z_buffer, State *state, Assets *assets, Vec3 pos, Vec3 rot, Vec3 scale)
{
    // PERSPECTIVE
    Mat4 model = mat4_create_mvp(
        pos, rot, scale, // model position, rotation, scale
        state->camera_pos, state->camera_target, state->camera_up,
        degrees_to_radians(90.0), (float)pb->width / (float)pb->height, 0.1f, 100.0f);

    // ISOMETRIC
    // Mat4 model = mat4_create_mvp_isometric_specific(
    //     vec3_create(0.0f, 0.0f, 0.0f),                     // model position
    //     vec3_create(0.0f, angle, 0.0f),                    // model rotation
    //     vec3_create(scalef, scalef, scalef),               // model scale
    //     (float)pb->width / (float)pb->height, 0.1f, 100.0f // aspect ratio, near plane, far plane
    // );

    SFA *transformed_vertices = sfa_transform_vertices(assets->gba_mesh->vertices, &model);

    // Perform perspective divide
    perspective_divide(transformed_vertices); // Implement perspective_divide

    // Map to screen coordinates with z
    SFA *screen_coords = sfa_new(transformed_vertices->length / 4 * 3);
    map_to_screen_keep_z(transformed_vertices, screen_coords, pb->width, pb->height);

    // Render lines
    // draw_tris_lines(pb, screen_coords, assets->gba_mesh->indices, COLOR_WHITE);
    // draw_tris_with_colors_and_face_numbers(
    //     pb, assets->charmap_white,
    //     screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors, 1.0, COLOR_BLACK);
    // draw_tris_with_colors(pb, screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors);
    draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors);

    // Cleanup
    sfa_free(transformed_vertices);
    sfa_free(screen_coords);
}

void draw(PixelBuffer *pb, FTexture *z_buffer, State *state, Assets *assets)
{
    // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);
    draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);
    Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
    draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFFFFFFFF);

    // float scalef = 0.1;
    float scalef = 20.0;
    float time = SDL_GetTicks() / 1000.0f;
    float angle = time * 0.5f;

    // draw_gba_mesh(pb, z_buffer, state, assets,
    //               vec3_create(0.0f, 0.0f, 0.0f),      // position
    //               vec3_create(0.0f, angle, 0.0f),     // rotation
    //               vec3_create(scalef, scalef, scalef) // scale
    // );

    // draw a 4x4 grid of gba meshes, centered at the origin though
    int grid_size = 4;
    float grid_spacing = 100.0f;
    float half_grid = (grid_size - 1) * grid_spacing * 0.5f;
    for (int y = 0; y < grid_size; y++)
    {
        for (int x = 0; x < grid_size; x++)
        {
            float x_pos = x * grid_spacing - half_grid;
            float y_pos = y * grid_spacing - half_grid;
            draw_gba_mesh(pb, z_buffer, state, assets,
                          vec3_create(x_pos, 0.0f, y_pos),    // position
                          vec3_create(0.0f, angle, 0.0f),     // rotation
                          vec3_create(scalef, scalef, scalef) // scale
            );
        }
    }

    {

        // debug the z buffer
        // get min and max z
        float min_z = FLT_MAX;
        float max_z = -FLT_MAX;
        for (int i = 0; i < z_buffer->width * z_buffer->height; i++)
        {
            float z = z_buffer->data[i];
            if (z < min_z)
            {
                min_z = z;
            }
            if (z > max_z)
            {
                max_z = z;
            }
        }

        float mapscale = 2.0f;
        for (int y = 0; y < z_buffer->height; y += (int)mapscale)
        {
            for (int x = 0; x < z_buffer->width; x += (int)mapscale)
            {
                float z = f_texture_get(z_buffer, x, y);
                // z = map_range(z, min_z, max_z, 0.0f, 1.0f);
                // z = 1.0f - z;
                // high pow
                // z = pow(z, 1000.0f);
                z /= 300.0f;
                uint8_t z8 = (uint8_t)(z * 255.0f);
                uint32_t color = from_rgb(z8, z8, z8);
                set_pixel(pb, x / mapscale, y / mapscale, color);

                // if (z < 500.0f)
                // {
                //     // write out in text the z value on screen
                //     char z_str[10];
                //     sprintf(z_str, "%.1f", z);
                //     blit_string(pb, assets->charmap_white, z_str, x, y, 1, COLOR_WHITE);
                // }
            }
        }
    }
}