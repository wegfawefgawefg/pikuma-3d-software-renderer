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
    draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);

    Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
    draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFFFFFFFF);

    // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);

    // make a triangular prism out of tris
    float size = 40.0;
    Mesh *mesh = mesh_prim_tetrahedron(size);
    // rotate based on time. get from sdl
    float time = SDL_GetTicks() / 1000.0f;
    Vec3 rotation = vec3_create(time, time, time);
    sfa_rotate(mesh->vertices, rotation);
    // move it to the center of the screen
    Vec3 translation = vec3_create(pb->width / 2, pb->height / 2, 20);
    mesh_translate(mesh, translation);
    // project to isometric
    // project to orthographic
    SFA *ortho_projected_shape = sfa_orthographic_project(mesh->vertices);

    // draw it
    draw_tris_lines(pb, ortho_projected_shape, mesh->indices, COLOR_WHITE);

    sfa_free(ortho_projected_shape);
    mesh_free(mesh);
}