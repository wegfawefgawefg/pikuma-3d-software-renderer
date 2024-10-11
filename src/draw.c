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

void draw_gba_mesh(
    PixelBuffer *pb, FTexture *z_buffer,
    State *state, Assets *assets,
    Light *lights, uint8_t num_lights,
    Vec3 pos, Vec3 rot, Vec3 scale)
{
    Mat4 model = mat4_create_model(pos, rot, scale);
    // SFA *model_transformed_vertices = sfa_transform_vertices(assets->gba_mesh->vertices, &model);

    // bool model_too_far_from_lights = false;
    // // get first vert
    // Vec3 first_vert = vec3_create(
    //     model_transformed_vertices->data[0],
    //     model_transformed_vertices->data[1],
    //     model_transformed_vertices->data[2]);
    // for (int i = 0; i < num_lights; i++)
    // {
    //     float distance = vec3_distance(lights[i].pos, first_vert);
    //     if (distance > 600)
    //     {
    //         model_too_far_from_lights = true;
    //         break;
    //     }
    // }

    // SU32A *colors = NULL;
    // if (!model_too_far_from_lights)
    // {
    //     colors = lighting_get_face_colors(
    //         model_transformed_vertices,
    //         assets->gba_mesh->indices,
    //         assets->gba_mesh->colors,
    //         lights, num_lights);
    // }
    // else
    // {
    //     colors = assets->gba_mesh->colors;
    // }
    // sfa_free(model_transformed_vertices);

    Mat4 vp = mat4_create_vp(
        state->camera_pos, state->camera_target, state->camera_up,
        degrees_to_radians(90.0), (float)pb->width / (float)pb->height, 0.1f, 100.0f);

    Mat4 mvp = mat4_multiply(vp, model);

    SFA *transformed_vertices = sfa_transform_vertices(assets->gba_mesh->vertices, &mvp);

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
    // draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors);
    // use the colors calculated from the lighting
    draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors);

    // Cleanup
    sfa_free(transformed_vertices);
    sfa_free(screen_coords);

    // if (!model_too_far_from_lights)
    // {
    //     su32a_free(colors);
    // }
}

void draw(PixelBuffer *pb, FTexture *z_buffer, State *state, Assets *assets)
{
    // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);
    draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);
    Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
    draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFFFFFFFF);

    // make an array of 8 lights with malloc
    uint8_t num_lights = 4;
    Light *lights = (Light *)malloc(num_lights * sizeof(Light));

    // put a light at x 0, y 100, z 0
    // lights[0] = light_new(vec3_create(0.0f, 100.0f, 0.0f), COLOR_GREEN, 20.0f);
    // lights[1] = light_new(vec3_create(100.0f, 100.0f, 0.0f), COLOR_RED, 20.0f);

    // make lights, and start them in random positions, spin them around the origin
    for (int i = 0; i < num_lights; i++)
    {
        float time = SDL_GetTicks() / 1000.0f;
        float radius = 300.0f;
        float height_variance = 10.0f;
        float angle = (float)i * (360.0f / (float)num_lights) + time * 30.0f;
        float x = cosf(degrees_to_radians(angle)) * radius;
        float y = sinf(degrees_to_radians(angle)) * height_variance;
        float z = sinf(degrees_to_radians(angle)) * radius;

        // Generate independent R, G, B using hash function with different seeds
        float r = hash(i + 1); // Seed offset for variation
        float g = hash(i + 2);
        float b = hash(i + 3);

        lights[i] = light_new(
            vec3_create(x, y, z),
            color_from_rgb(r * 255, g * 255, b * 255),
            10000.0f);
    }

    // float scalef = 0.1;
    float scalef = 20.0;
    float time = SDL_GetTicks() / 1000.0f;
    float angle = time * 0.5f;
    // float angle = 0.0f;

    // draw_gba_mesh(pb, z_buffer, state, assets,
    //               vec3_create(0.0f, 0.0f, 0.0f),      // position
    //               vec3_create(0.0f, angle, 0.0f),     // rotation
    //               vec3_create(scalef, scalef, scalef) // scale
    // );

    // draw a 4x4 grid of gba meshes, centered at the origin though
    // int grid_size = 12;
    // float grid_spacing = 100.0f;
    // float half_grid = (grid_size - 1) * grid_spacing * 0.5f;
    // for (int y = 0; y < grid_size; y++)
    // {
    //     for (int x = 0; x < grid_size; x++)
    //     {
    //         float x_pos = x * grid_spacing - half_grid;
    //         float y_pos = y * grid_spacing - half_grid;
    //         draw_gba_mesh(pb, z_buffer, state, assets, lights, num_lights,
    //                       vec3_create(x_pos, 0.0f, y_pos),    // position
    //                       vec3_create(0.0f, angle, 0.0f),     // rotation
    //                       vec3_create(scalef, scalef, scalef) // scale
    //         );
    //     }
    // }

    // draw a nxnxn cube grid of gba meshes, centered at the origin
    int grid_size = 8;
    float grid_spacing = 100.0f;
    float half_grid = (grid_size - 1) * grid_spacing * 0.5f;
    for (int z = 0; z < grid_size; z++)
    {
        for (int y = 0; y < grid_size; y++)
        {
            for (int x = 0; x < grid_size; x++)
            {
                float x_pos = x * grid_spacing - half_grid;
                float y_pos = y * grid_spacing - half_grid;
                float z_pos = z * grid_spacing - half_grid;
                draw_gba_mesh(pb, z_buffer, state, assets, lights, num_lights,
                              vec3_create(x_pos, y_pos, z_pos),   // position
                              vec3_create(0.0f, angle, 0.0f),     // rotation
                              vec3_create(scalef, scalef, scalef) // scale
                );
            }
        }
    }

    // draw a tiny gba at each light
    float light_gba_scale = 3.0f;
    for (int i = 0; i < num_lights; i++)
    {
        draw_gba_mesh(pb, z_buffer, state, assets, lights, num_lights,
                      lights[i].pos,                                                 // position
                      vec3_create(0.0f, 0.0f, 0.0f),                                 // rotation
                      vec3_create(light_gba_scale, light_gba_scale, light_gba_scale) // scale
        );
    }

    // free lights
    free(lights);

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

        float mapscale = 4.0f;
        for (int y = 0; y < z_buffer->height; y += (int)mapscale)
        {
            for (int x = 0; x < z_buffer->width; x += (int)mapscale)
            {
                float z = f_texture_get(z_buffer, x, y);
                // z = map_range(z, min_z, max_z, 0.0f, 1.0f);
                // z = 1.0f - z;
                // high pow
                // z = pow(z, 1000.0f);
                z /= 500.0f;
                uint8_t z8 = (uint8_t)(z * 255.0f);
                uint32_t color = color_from_rgb(z8, z8, z8);
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