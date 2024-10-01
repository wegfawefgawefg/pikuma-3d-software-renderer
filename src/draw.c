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

void draw(PixelBuffer *pb, State *state, Assets *assets)
{
    {
        draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);

        Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
        draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFFFFFFFF);

        // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);
    }

    {

        ///////// LIGHT SOURCE //////////////////////////////////
        float size = state->scale / 2.0;
        Mesh *light_mesh = mesh_prim_tetrahedron(size);

        // rotate based on time. get from sdl
        float time = SDL_GetTicks() / 1000.0f;
        Vec3 rotation = vec3_create(0, time, 0);
        sfa_rotate(light_mesh->vertices, rotation);

        // shift -z half size
        Vec3 translation = vec3_create(0, 0, -size);
        mesh_translate(light_mesh, translation);

        // shift x and z half size
        Vec3 translation2 = vec3_create(size, 0, size);
        mesh_translate(light_mesh, translation2);

        // translate based on camera
        mesh_translate(light_mesh, state->camera_pos);

        // orbit around 5,0,5
        float orbit_radius = 5.0 * state->scale;
        float x_offset = sin(time) * orbit_radius;
        float z_offset = cos(time) * orbit_radius;
        Vec3 orbit_translation = vec3_create(x_offset, 0, z_offset);
        mesh_translate(light_mesh, orbit_translation);

        // light should shift up and down with time on y
        float height_offset = sin(time * 2.0) * 20.0;
        Vec3 height_translation = vec3_create(0, height_offset, 0);
        mesh_translate(light_mesh, height_translation);

        // calculate half current grid width
        int grid_width = 10;
        int scaled_grid_width = grid_width * state->scale;
        int half_grid_width = scaled_grid_width / 2;
        Vec3 grid_center = vec3_create(half_grid_width, 0, half_grid_width);
        Vec3 grid_center_translation = vec3_create(grid_center.x, 0, grid_center.z);
        mesh_translate(light_mesh, grid_center_translation);

        ///////// END LIGHT SOURCE //////////////////////////////////

        // lets do a grid of rectangles with random colors
        int grid_height = 10;
        int rect_width = state->scale;

        // make a rectangle object
        Mesh *tl_rect = mesh_prim_rectangle(rect_width, rect_width);
        // rotate it so it is x,z plane , rotate 90 on x
        Vec3 rect_rotation = vec3_create(degrees_to_radians(90), 0, 0);
        sfa_rotate(tl_rect->vertices, rect_rotation);

        // move it it so the tl corner is at 0,0
        Vec3 rect_translation = vec3_create(rect_width / 2, rect_width / 2, 0);
        mesh_translate(tl_rect, rect_translation);

        // translate based on camera
        mesh_translate(tl_rect, state->camera_pos);

        // make a copy of mesh at position 0,0
        for (int x = 0; x < grid_width; x++)
        {
            for (int z = 0; z < grid_height; z++)
            {
                // translate up and down based on ripple center and ripple magnitude, and distance to ripple center
                Vec2 ripple_center = state->ripple_center;
                Vec2 cpos = vec2_create(x, z);
                float ripple_magnitude = state->ripple_magnitude;
                float distance_to_ripple = vec2_distance(ripple_center, cpos);

                // generate color from index
                // int xy = (((x + 1) * (z + 1)) * 1000) % 256;
                // uint32_t color = from_rgb(xy, xy, xy);
                uint32_t color = COLOR_WHITE;
                // uint32_t color = COLOR_GREEN;
                Vec2 tile_pos = vec2_create(x * rect_width, z * rect_width);

                // move the rect to the current position
                Mesh *rect = mesh_copy(tl_rect);
                translation = vec3_create(tile_pos.x, 0, tile_pos.y);
                mesh_translate(rect, translation);

                // translate up and down on y with time
                // float time = SDL_GetTicks() / 1000.0f;
                // float y_offset = sin(time * 10.0 + x + z) * 2.0;
                // Vec3 y_translation = vec3_create(0, y_offset, 0);
                // mesh_translate(rect, y_translation);

                // should decay based on distance squared
                float ripple_power = 1.5 * state->scale;
                float distance_factor = 0.06 * state->scale;
                float dist_sq = distance_to_ripple * distance_to_ripple;
                float y_offset = sin(time * 20.0) * ripple_magnitude * ripple_power / (dist_sq * distance_factor + 1.0);
                Vec3 y_translation = vec3_create(0, y_offset, 0);
                mesh_translate(rect, y_translation);

                // project
                // SFA *projected_rect = sfa_orthographic_project(rect->vertices);
                SFA *projected_rect = sfa_isometric_project(rect->vertices);

                // calculate lighting based on dist to light
                Vec3 light_mesh_center = mesh_get_center(light_mesh);
                Vec3 tile_mesh_center = mesh_get_center(rect);
                float dist_to_light = vec3_distance(light_mesh_center, tile_mesh_center);
                // make sure dist is only positive, and not 0
                float light_power = 255.0 / ((dist_to_light * dist_to_light) * 0.2);
                Vec3 v3color = vec3_fmul(color_to_vec3(color), light_power);
                // clamp it to 255
                v3color.x = fmin(255.0, fmax(0.0, v3color.x));
                v3color.y = fmin(255.0, fmax(0.0, v3color.y));
                v3color.z = fmin(255.0, fmax(0.0, v3color.z));
                color = vec3_to_color(v3color);

                // draw it
                draw_tris(pb, projected_rect, rect->indices, color);

                sfa_free(projected_rect);
                mesh_free(rect);
            }
        }

        mesh_free(tl_rect);

        // project
        // SFA *projected_shape = sfa_orthographic_project(mesh->vertices);
        SFA *projected_light_shape = sfa_isometric_project(light_mesh->vertices);

        // draw it
        draw_tris_lines(pb, projected_light_shape, light_mesh->indices, COLOR_NEON_GREEN);

        sfa_free(projected_light_shape);

        // free light mesh
        mesh_free(light_mesh);
    }

    {

        // make a triangular prism out of tris
        float size = state->scale / 2.0;
        Mesh *mesh = mesh_prim_tetrahedron(size);

        // rotate based on time. get from sdl
        float time = SDL_GetTicks() / 1000.0f;
        Vec3 rotation = vec3_create(0, 0, time);
        sfa_rotate(mesh->vertices, rotation);

        // shift -z half size
        Vec3 translation = vec3_create(0, 0, -size);
        mesh_translate(mesh, translation);

        // shift x and z half size
        Vec3 translation2 = vec3_create(size, 0, size);
        mesh_translate(mesh, translation2);

        // translate based on camera
        mesh_translate(mesh, state->camera_pos);

        // translate based on state->pointer_pos
        Vec3 pos = vec3_create(state->pointer_pos.x * state->scale, 0, state->pointer_pos.y * state->scale);
        mesh_translate(mesh, pos);

        // project
        // SFA *projected_shape = sfa_orthographic_project(mesh->vertices);
        SFA *projected_shape = sfa_isometric_project(mesh->vertices);

        // draw it
        draw_tris_lines(pb, projected_shape, mesh->indices, COLOR_NEON_BLUE);

        sfa_free(projected_shape);
        mesh_free(mesh);
    }

    // this is the light
    // lets make another prism that orbits around 5,0,5 at a distance of 5
    // use sin for x offset and cos for z offset
    // needs to factor in scale, camera pos, and pointer pos
    // so the object should appear to orbit around the center of the grid
    {
    }
}