#include "draw.h"
#include "primitives.h"
#include "input.h"
#include "colors.h"
#include "pixel_buffer.h"
#include "assets.h"

void draw(PixelBuffer *pb, State *state, Assets *assets)
{
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

    IVec2 mousePos = get_mouse_pos();
    draw_cursor(pb, mousePos.x, mousePos.y, 10, 0xFFFFFFFF);
    // rotate based on time

    IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);
    float rotation = SDL_GetTicks() / 1000.0f * 45.0f;
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, screen_center, rotation, vec2_create(0, 0));
    float scale = 1.0f + 0.5f * sin(SDL_GetTicks() / 1000.0f * 2.0f * M_PI);
    // blit_with_scale(assets->pointer_pixel_buffer, pb, screen_center, vec2_create(scale, scale));
    blit_with_scale_and_rotation(assets->pointer_pixel_buffer, pb, screen_center, vec2_create(scale, scale), rotation,
                                 // tl
                                 vec2_create(0, 0)
                                 // center
                                 //  ivec2_to_vec2(get_center_of_pixelbuffer(assets->pointer_pixel_buffer))
                                 // screen center
                                 //  vec2_create(pb->width / 2, pb->height / 2)
                                 //
    );

    // draw 4 pointers  spinning around where the mouse points
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation, vec2_create(0, 0));
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation + 90, vec2_create(0, 0));
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation + 180, vec2_create(0, 0));
    // blit_with_rotation(assets->pointer_pixel_buffer, pb, mousePos, rotation + 270, vec2_create(0, 0));
}