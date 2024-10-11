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

// Simple hash function to generate pseudo-random float between 0 and 1
float hash(int x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return ((float)x) / 4294967295.0f;
}

// ISOMETRIC
// Mat4 model = mat4_create_mvp_isometric_specific(
//     vec3_create(0.0f, 0.0f, 0.0f),                     // model position
//     vec3_create(0.0f, angle, 0.0f),                    // model rotation
//     vec3_create(scalef, scalef, scalef),               // model scale
//     (float)pb->width / (float)pb->height, 0.1f, 100.0f // aspect ratio, near plane, far plane
// );

// draw the gba mesh
void draw_gba_mesh(
    PixelBuffer *pb, FTexture *z_buffer,
    State *state, Assets *assets,
    Light *lights, uint8_t num_lights,
    Vec3 pos, Vec3 rot, Vec3 scale)
{
    // PERSPECTIVE
    // Mat4 model = mat4_create_mvp(
    //     pos, rot, scale, // model position, rotation, scale
    //     state->camera_pos, state->camera_target, state->camera_up,
    //     degrees_to_radians(90.0), (float)pb->width / (float)pb->height, 0.1f, 100.0f);
    Mat4 model = mat4_create_model(pos, rot, scale);

    SFA *model_transformed_vertices = sfa_transform_vertices(assets->gba_mesh->vertices, &model);

    // lighting calculations
    // for now ignore the light color and just use the brightness
    // go through each tri, calculate the centroid, distance to light, and brightness
    SU32A *colors = su32a_new(assets->gba_mesh->colors->length);
    {
        int num_faces = assets->gba_mesh->indices->length / 3;
        for (int i = 0; i < num_faces; i++)
        {
            int i0 = assets->gba_mesh->indices->data[i * 3 + 0];
            int i1 = assets->gba_mesh->indices->data[i * 3 + 1];
            int i2 = assets->gba_mesh->indices->data[i * 3 + 2];
            // int i3 = assets->gba_mesh->indices->data[i * 3 + 3];

            Vec3 a = vec3_create(
                model_transformed_vertices->data[i0 * 4 + 0],
                model_transformed_vertices->data[i0 * 4 + 1],
                model_transformed_vertices->data[i0 * 4 + 2]);

            Vec3 b = vec3_create(
                model_transformed_vertices->data[i1 * 4 + 0],
                model_transformed_vertices->data[i1 * 4 + 1],
                model_transformed_vertices->data[i1 * 4 + 2]);

            Vec3 c = vec3_create(
                model_transformed_vertices->data[i2 * 4 + 0],
                model_transformed_vertices->data[i2 * 4 + 1],
                model_transformed_vertices->data[i2 * 4 + 2]);

            Vec3 centroid = vec3_create(0.0f, 0.0f, 0.0f);
            centroid = vec3_add(centroid, a);
            centroid = vec3_add(centroid, b);
            centroid = vec3_add(centroid, c);
            centroid = vec3_div(centroid, 3.0f);

            Vec3 face_normal = vec3_cross(vec3_sub(b, a), vec3_sub(c, a));
            face_normal = vec3_normalize(face_normal);
            // invert face_normal
            // face_normal = vec3_mul(face_normal, -1.0f);

            // calculate the brightness
            // for now ignore normal, just do distance
            // float brightness = 0.0f;
            uint32_t light_color_sum = 0;
            const float dist_factor = 1.0f;
            for (int j = 0; j < num_lights; j++)
            {
                Light light = lights[j];
                Vec3 light_dir = vec3_sub(light.pos, centroid);
                float distance = vec3_length(light_dir);
                float dot = vec3_dot(face_normal, vec3_normalize(light_dir));
                dot = fmax(0.0f, dot);

                float magnitude = 1.0f / ((distance * distance) * dist_factor);
                magnitude = fmin(1.0f, magnitude);
                magnitude = fmax(0.0f, magnitude);

                magnitude *= dot;
                magnitude *= light.brightness;

                // clamp again
                magnitude = fmin(1.0f, magnitude);
                magnitude = fmax(0.0f, magnitude);

                // calculate this lights influence
                uint32_t this_light_color = color_fmul(light.color, magnitude);
                light_color_sum = color_add(light_color_sum, this_light_color);
            }
            const float ambient_factor = 0.01f;
            uint32_t color = assets->gba_mesh->colors->data[i];
            uint32_t color_f = color_fmul(color, ambient_factor);
            uint32_t col = color_add(color_f, light_color_sum);
            colors->data[i] = col;
        }
    }
    sfa_free(model_transformed_vertices);

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
    draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, assets->gba_mesh->indices, colors);

    // Cleanup
    sfa_free(transformed_vertices);
    sfa_free(screen_coords);

    su32a_free(colors);
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
    int grid_size = 8;
    float grid_spacing = 100.0f;
    float half_grid = (grid_size - 1) * grid_spacing * 0.5f;
    for (int y = 0; y < grid_size; y++)
    {
        for (int x = 0; x < grid_size; x++)
        {
            float x_pos = x * grid_spacing - half_grid;
            float y_pos = y * grid_spacing - half_grid;
            draw_gba_mesh(pb, z_buffer, state, assets, lights, num_lights,
                          vec3_create(x_pos, 0.0f, y_pos),    // position
                          vec3_create(0.0f, angle, 0.0f),     // rotation
                          vec3_create(scalef, scalef, scalef) // scale
            );
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