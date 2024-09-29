#include <math.h>

#include "draw.h"
#include "primitives.h"
#include "input.h"
#include "colors.h"
#include "pixel_buffer.h"
#include "assets.h"
#include "utils.h"

void draw(PixelBuffer *pb, State *state, Assets *assets)
{
    IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);

    // draw a grey grid on the back
    draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);
    // draw_checkerboard(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, 0xFF333333, 0xFF444444);
    // draw_checkerboard(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20,
    //                   COLOR_GRAY_DARK,
    //                   COLOR_TRANS_PURPLE);

    // draw the triangles
    // for (int i = 0; i < NUM_TRIANGLES; i++)
    // {
    //     MetaTriangle *mt = &state->triangles[i];
    //     draw_triangle_lines(pb, mt->t, mt->color);
    // }

    // put a light at the center of the screen but -200 z
    Vec3 light = vec3_create(screen_center.x, screen_center.y, -200);
    // SPINNING CUBE
    {

        // make a cube
        Vec2 screen_center_f = ivec2_to_vec2(screen_center);
        // one tenth of the screen
        int quad_size = pb->width / 5;
        Quads *quads = create_quads_cube(vec3_create(screen_center_f.x, screen_center_f.y, 0), quad_size);

        // rotate the cube
        float quad_rotation = SDL_GetTicks() / 1000.0f * 45.0f + 13.0f;
        float quad_rotation_radians = degrees_to_radians(quad_rotation);
        rotate_quads(quads, vec3_create(screen_center_f.x, screen_center_f.y, 0), vec3_create(quad_rotation_radians, quad_rotation_radians, quad_rotation_radians));

        // shift them down one 8th of the screen
        Vec3 shift = vec3_create(0, pb->height / 8, 0);
        shift_quads(quads, shift);

        // zsort the quads
        zsort_quads(quads);

        // draw it
        for (int i = 0; i < quads->count; i++)
        {
            Quad *quad = &quads->quads[i];
            // void draw_ortho_quad_lines(PixelBuffer *pb, Quad *quad, uint32_t color);
            // draw_ortho_quad_lines(pb, quad, 0xFFFFFFFF);
            // draw_ortho_quad(pb, quad, 0xFFFFFFFF);

            // get the normal
            Vec3 normal = get_quad_normal(quad);

            // get the light direction
            Vec3 center = get_quad_center(quad);
            Vec3 light_dir = vec3_sub(light, center);
            light_dir = vec3_normalize(light_dir);
            float dp = vec3_dot(normal, light_dir);
            dp = fmax(0.0, dp);

            // get the color
            uint32_t color = from_rgba(255 * dp, 255 * dp, 255 * dp, 200);

            draw_ortho_quad(pb, quad, color);
        }

        destroy_quads(quads);
    }

    // WE CAN DO OTHER SHAPES
    // Quads *create_quads_cube(Vec3 center, float size);
    // Quads *create_quads_pyramid(Vec3 center, float base_size, float height);
    // Quads *create_quads_prism(Vec3 center, float base_width, float base_depth, float height);
    // Quads *create_quads_octahedron(Vec3 center, float size);

    // LETS DO OCTAHEDRON
    {
        Vec2 screen_center_f = ivec2_to_vec2(screen_center);
        // one tenth of the screen
        int octahedron_size = pb->width / 5;
        Vec2 octahedron_center = vec2_create(screen_center_f.x * 1.5, screen_center_f.y);
        Quads *quads = create_quads_octahedron(vec3_create(octahedron_center.x, octahedron_center.y, 0), octahedron_size);

        // rotate the octahedron
        float octahedron_rotation = SDL_GetTicks() / 1000.0f * 45.0f + 45.0f;
        float octahedron_rotation_radians = degrees_to_radians(octahedron_rotation);
        rotate_quads(quads, vec3_create(octahedron_center.x, octahedron_center.y, 0),
                     vec3_create(octahedron_rotation_radians, octahedron_rotation_radians, octahedron_rotation_radians));

        // shift them down one 8th of the screen
        Vec3 shift = vec3_create(0, pb->height / 8, 0);
        shift_quads(quads, shift);

        // zsort the quads
        zsort_quads(quads);

        // draw it
        for (int i = 0; i < quads->count; i++)
        {
            Quad *quad = &quads->quads[i];
            // void draw_ortho_quad_lines(PixelBuffer *pb, Quad *quad, uint32_t color);
            // draw_ortho_quad_lines(pb, quad, 0xFFFFFFFF);
            // draw_ortho_quad(pb, quad, 0xFFFFFFFF);

            // get the normal
            Vec3 normal = get_quad_normal(quad);

            // get the light direction
            Vec3 center = get_quad_center(quad);
            Vec3 light_dir = vec3_sub(light, center);
            light_dir = vec3_normalize(light_dir);
            float dp = vec3_dot(normal, light_dir);
            dp = fmax(0.0, dp);

            // get the color
            uint32_t color = set_alpha(color_fmul(COLOR_NEON_PURPLE, dp), 200);

            draw_ortho_quad(pb, quad, color);
        }

        destroy_quads(quads);
    }

    // LETS DO A PRISM
    {
        Vec2 screen_center_f = ivec2_to_vec2(screen_center);
        // one tenth of the screen
        int prism_width = pb->width / 5;
        int prism_depth = pb->width / 10;
        int prism_height = pb->width / 5;
        Vec2 prism_center = vec2_create(screen_center_f.x * 0.5, screen_center_f.y);
        Quads *quads = create_quads_prism(vec3_create(prism_center.x, prism_center.y, 0), prism_width, prism_depth, prism_height);

        // rotate the prism
        float prism_rotation = SDL_GetTicks() / 1000.0f * 45.0f + 72.0f;
        float prism_rotation_radians = degrees_to_radians(prism_rotation);
        rotate_quads(quads, vec3_create(prism_center.x, prism_center.y, 0), vec3_create(prism_rotation_radians, prism_rotation_radians, prism_rotation_radians));

        // shift them down one 8th of the screen
        Vec3 shift = vec3_create(0, pb->height / 8, 0);
        shift_quads(quads, shift);

        // zsort the quads
        zsort_quads(quads);

        // draw it
        for (int i = 0; i < quads->count; i++)
        {
            Quad *quad = &quads->quads[i];
            // void draw_ortho_quad_lines(PixelBuffer *pb, Quad *quad, uint32_t color);
            // draw_ortho_quad_lines(pb, quad, 0xFFFFFFFF);
            // draw_ortho_quad(pb, quad, 0xFFFFFFFF);

            // get the normal
            Vec3 normal = get_quad_normal(quad);

            // get the light direction
            Vec3 center = get_quad_center(quad);
            Vec3 light_dir = vec3_sub(light, center);
            light_dir = vec3_normalize(light_dir);
            float dp = vec3_dot(normal, light_dir);
            dp = fmax(0.0, dp);

            uint32_t color = set_alpha(color_fmul(COLOR_YELLOW, dp), 200);
            draw_ortho_quad(pb, quad, color);
        }

        destroy_quads(quads);
    }

    // LETS DO AN ARROW
    // put it pointing at the center, and make it as long as 1/6th the screen, and 1/10th the screen wide
    // put it at the center of the screen.
    // you can use:
    // Quads *create_quads_arrow(float length, float width);
    // void shift_quads(Quads *quads, Vec3 shift);

    {

        // one tenth of the screen
        int arrow_length = pb->width / 6;
        int arrow_width = pb->width / 10;
        Quads *quads = create_quads_arrow(arrow_length, arrow_width);

        // arrow x position is 1/2th the screen center * (float)state.selection
        // float arrow_x = (float)screen_center.x / 2.0 * (float)(state->selection + 1);
        // float arrow_y = (float)screen_center.y;
        Vec2 screen_center_f = vec2_create(screen_center.x, screen_center.y);
        // Vec3 shift = vec3_create(screen_center_f.x, screen_center_f.y, 0);
        // shift_quads(quads, shift);

        // debug rotate
        // rotate around the x axis 90 degrees
        rotate_quads(quads, vec3_create(screen_center_f.x, screen_center_f.y, 0), vec3_create(degrees_to_radians(-90), 0, 0));

        // set the x position based on the selection
        float arrow_x = screen_center_f.x * 0.5 * (float)(state->selection + 1);
        Vec3 shift = vec3_create(arrow_x, 0.0, 0.0);
        shift_quads(quads, shift);

        // oscillate it up and down a little bit too
        float arrow_oscilation_t = SDL_GetTicks() / 1000.0f * 10.0f;
        float arrow_oscilation = sin(arrow_oscilation_t) * screen_center.y / 10.0f;
        Vec3 vertical_oscilation_shift = vec3_create(0, arrow_oscilation, 0);
        shift_quads(quads, vertical_oscilation_shift);

        // now need an oscilating rotation, use sin
        float arrow_rotation_oscilation = SDL_GetTicks() / 1000.0f * 45.0f * 4.0f;
        float arrow_rotation_oscilation_radians = degrees_to_radians(arrow_rotation_oscilation);
        arrow_rotation_oscilation_radians = sin(arrow_rotation_oscilation_radians) * 0.2;
        Vec3 shift_sum = vec3_add(shift, vertical_oscilation_shift);
        rotate_quads(quads, vec3_create(shift_sum.x, shift_sum.y, 0), vec3_create(0.0, arrow_rotation_oscilation_radians, arrow_rotation_oscilation_radians));

        // zsort the quads
        zsort_quads(quads);

        // draw it
        for (int i = 0; i < quads->count; i++)
        {
            Quad *quad = &quads->quads[i];
            // void draw_ortho_quad_lines(PixelBuffer *pb, Quad *quad, uint32_t color);
            // draw_ortho_quad_lines(pb, quad, 0xFFFFFFFF);
            // draw_ortho_quad(pb, quad, 0xFFFFFFFF);

            // get the normal
            Vec3 normal = get_quad_normal(quad);

            // get the light direction
            Vec3 center = get_quad_center(quad);
            Vec3 light_dir = vec3_sub(light, center);
            light_dir = vec3_normalize(light_dir);
            float dp = vec3_dot(normal, light_dir);
            dp = fmax(0.0, dp);

            // get the color
            uint32_t color = from_rgba(255 * dp, 0, 0, 255);

            draw_ortho_quad(pb, quad, color);
        }

        destroy_quads(quads);
    }

    // SPINNING POINTER
    // IVec2 mousePos = get_mouse_pos();
    // draw_cursor(pb, mousePos.x, mousePos.y, 10, 0xFFFFFFFF);
    // // rotate based on time

    // float rotation = SDL_GetTicks() / 1000.0f * 45.0f;
    // // blit_with_rotation(assets->pointer_pixel_buffer, pb, screen_center, rotation, vec2_create(0, 0));
    // float scale = 1.0f + 0.5f * sin(SDL_GetTicks() / 1000.0f * 2.0f * M_PI);
    // // blit_with_scale(assets->pointer_pixel_buffer, pb, screen_center, vec2_create(scale, scale));
    // blit_with_scale_and_rotation(assets->pointer_pixel_buffer, pb, screen_center, vec2_create(scale, scale), rotation,
    //                              // tl
    //                              vec2_create(0, 0)
    //                              // center
    //                              //  ivec2_to_vec2(get_center_of_pixelbuffer(assets->pointer_pixel_buffer))
    //                              // screen center
    //                              //  vec2_create(pb->width / 2, pb->height / 2)
    //                              //
    // );

    // draw 4 pointers  spinning around where the mouse points
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation, vec2_create(0, 0));
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation + 90, vec2_create(0, 0));
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation + 180, vec2_create(0, 0));
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation + 270, vec2_create(0, 0));
}