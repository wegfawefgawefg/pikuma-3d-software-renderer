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

void draw(PixelBuffer *pb, State *state, Assets *assets)
{
    {
        draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);

        Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
        draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFFFFFFFF);

        // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);

        // draw the gba mesh
        {
            float scalef = 20.0 * RENDER_SCALE;
            // float scalef = 5.0;
            float time = SDL_GetTicks() / 1000.0f;
            float angle = time * 0.5f;

            // PERSPECTIVE
            Mat4 model = mat4_create_mvp(
                vec3_create(0.0f, 0.0f, 0.0f),       // model position
                vec3_create(0.0f, angle, 0.0f),      // model rotation
                vec3_create(scalef, scalef, scalef), // model scale
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

            // Map to screen coordinates
            SFA *screen_coords = sfa_new(transformed_vertices->length / 4 * 2);
            map_to_screen(transformed_vertices, screen_coords, pb->width, pb->height); // Implement map_to_screen

            // Render lines
            draw_tris_lines(pb, screen_coords, assets->gba_mesh->indices, COLOR_WHITE);

            // Cleanup
            sfa_free(transformed_vertices);
            sfa_free(screen_coords);
        }
    }
}