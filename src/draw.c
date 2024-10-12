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

#define COLOR_PICKER_SIZE 100
#define COLOR_PICKER_MARGIN 10

#define BRIGHTNESS_SLIDER_WIDTH 100
#define BRIGHTNESS_SLIDER_HEIGHT 30
#define BRIGHTNESS_SLIDER_MARGIN 10

void draw_mesh(
    PixelBuffer *pb, FTexture *z_buffer, PixelBuffer *face_buffer,
    State *state, Assets *assets,
    SFA *vertices, SU32A *indices, SU32A *colors,
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

    SFA *transformed_vertices = sfa_transform_vertices(vertices, &mvp);

    // Perform perspective divide
    perspective_divide(transformed_vertices); // Implement perspective_divide

    // Map to screen coordinates with z
    SFA *screen_coords = sfa_new(transformed_vertices->length / 4 * 3);
    map_to_screen_keep_z(transformed_vertices, screen_coords, pb->width, pb->height);

    // Render lines
    draw_tris_lines_with_depth(pb, screen_coords, assets->gba_mesh->indices, COLOR_WHITE);
    // draw_tris_with_colors_and_face_numbers(
    //     pb, assets->charmap_white,
    //     screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors, 1.0, COLOR_BLACK);
    // draw_tris_with_colors(pb, screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors);
    // draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, assets->gba_mesh->indices, assets->gba_mesh->colors);
    // use the colors calculated from the lighting
    // draw_tris_with_colors_and_depth(pb, z_buffer, screen_coords, indices, colors);
    // draw_tris_with_colors_and_depth_with_face_buffer(pb, z_buffer, face_buffer, screen_coords, indices, colors);

    // Cleanup
    sfa_free(transformed_vertices);
    sfa_free(screen_coords);

    // if (!model_too_far_from_lights)
    // {
    //     su32a_free(colors);
    // }
}

// function to draw mesh as wireframe

// Function to draw the 2D hue-saturation color picker
void draw_hue_saturation_picker(PixelBuffer *pb, State *state)
{
    // Define color picker position
    int picker_x = pb->width - COLOR_PICKER_SIZE - COLOR_PICKER_MARGIN;
    int picker_y = COLOR_PICKER_MARGIN;

    for (int x = 0; x < COLOR_PICKER_SIZE; x++)
    {
        // Calculate hue (0 to 360 degrees)
        float hue = ((float)x / (float)(COLOR_PICKER_SIZE - 1)) * 360.0f;

        for (int y = 0; y < COLOR_PICKER_SIZE; y++)
        {
            // Calculate saturation (1.0 at top to 0.0 at bottom)
            float saturation = 1.0f - ((float)y / (float)(COLOR_PICKER_SIZE - 1));

            // Use current brightness from state
            float brightness = state->brightness;

            // Convert HSV to RGB
            uint8_t r, g, b;
            hsv_to_rgb(hue, saturation, brightness, &r, &g, &b);

            // Pack RGB into uint32_t RGBA format (R << 24 | G << 16 | B << 8 | A)
            uint32_t rgb_packed = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFF; // A=255

            // Set the pixel color
            pixel_buffer_set(pb, picker_x + x, picker_y + y, rgb_packed);
        }
    }

    // Optional: Draw border around the color picker
    uint32_t border_color = 0xFFFFFFFF; // White border
    for (int x = 0; x < COLOR_PICKER_SIZE; x++)
    {
        pixel_buffer_set(pb, picker_x + x, picker_y, border_color);
        pixel_buffer_set(pb, picker_x + x, picker_y + COLOR_PICKER_SIZE - 1, border_color);
    }
    for (int y = 0; y < COLOR_PICKER_SIZE; y++)
    {
        pixel_buffer_set(pb, picker_x, picker_y + y, border_color);
        pixel_buffer_set(pb, picker_x + COLOR_PICKER_SIZE - 1, picker_y + y, border_color);
    }

    // Draw marker indicating current hue and saturation
    int marker_x = picker_x + (int)((state->hue / 360.0f) * (COLOR_PICKER_SIZE - 1));
    int marker_y = picker_y + (int)((1.0f - state->saturation) * (COLOR_PICKER_SIZE - 1));
    uint32_t marker_color = 0xFF000000; // Black marker

    // Draw a small crosshair
    for (int dx = -2; dx <= 2; dx++)
    {
        for (int dy = -2; dy <= 2; dy++)
        {
            int px = marker_x + dx;
            int py = marker_y + dy;
            if (px >= picker_x && px < picker_x + COLOR_PICKER_SIZE &&
                py >= picker_y && py < picker_y + COLOR_PICKER_SIZE)
            {
                pixel_buffer_set(pb, px, py, marker_color);
            }
        }
    }
}

// Function to draw the brightness slider
void draw_brightness_slider(PixelBuffer *pb, State *state)
{
    // Define brightness slider position
    int slider_x = pb->width - BRIGHTNESS_SLIDER_WIDTH - BRIGHTNESS_SLIDER_MARGIN;
    int slider_y = BRIGHTNESS_SLIDER_MARGIN + COLOR_PICKER_SIZE + 10; // 10 pixels below color picker

    for (int x = 0; x < BRIGHTNESS_SLIDER_WIDTH; x++)
    {
        // Calculate brightness (1.0 at left to 0.0 at right)
        float brightness = 1.0f - ((float)x / (float)(BRIGHTNESS_SLIDER_WIDTH - 1));

        // Use current hue and saturation from state
        float hue = state->hue;
        float saturation = state->saturation;

        // Convert HSV to RGB
        uint8_t r, g, b;
        hsv_to_rgb(hue, saturation, brightness, &r, &g, &b);

        // Pack RGB into uint32_t RGBA format (R << 24 | G << 16 | B << 8 | A)
        uint32_t rgb_packed = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFF; // A=255

        // Set the pixel color across the slider's height
        for (int y = 0; y < BRIGHTNESS_SLIDER_HEIGHT; y++)
        {
            pixel_buffer_set(pb, slider_x + x, slider_y + y, rgb_packed);
        }
    }

    // Optional: Draw border around the brightness slider
    uint32_t border_color = 0xFFFFFFFF; // White border
    for (int x = 0; x < BRIGHTNESS_SLIDER_WIDTH; x++)
    {
        pixel_buffer_set(pb, slider_x + x, slider_y, border_color);
        pixel_buffer_set(pb, slider_x + x, slider_y + BRIGHTNESS_SLIDER_HEIGHT - 1, border_color);
    }
    for (int y = 0; y < BRIGHTNESS_SLIDER_HEIGHT; y++)
    {
        pixel_buffer_set(pb, slider_x, slider_y + y, border_color);
        pixel_buffer_set(pb, slider_x + BRIGHTNESS_SLIDER_WIDTH - 1, slider_y + y, border_color);
    }

    // Draw marker indicating current brightness
    int marker_x = slider_x + (int)((1.0f - state->brightness) * (BRIGHTNESS_SLIDER_WIDTH - 1));
    int marker_y = slider_y + BRIGHTNESS_SLIDER_HEIGHT / 2;
    uint32_t marker_color = 0xFF000000; // Black marker

    // Draw a small vertical line as marker
    for (int dy = -2; dy <= 2; dy++)
    {
        int px = marker_x;
        int py = marker_y + dy;
        if (px >= slider_x && px < slider_x + BRIGHTNESS_SLIDER_WIDTH &&
            py >= slider_y && py < slider_y + BRIGHTNESS_SLIDER_HEIGHT)
        {
            pixel_buffer_set(pb, px, py, marker_color);
        }
    }
}

void draw_selected_color(PixelBuffer *pb, uint32_t color)
{
    int color_display_size = 30;
    int display_x = COLOR_PICKER_MARGIN;
    int display_y = COLOR_PICKER_MARGIN;

    for (int x = 0; x < color_display_size; x++)
    {
        for (int y = 0; y < color_display_size; y++)
        {
            pixel_buffer_set(pb, display_x + x, display_y + y, color);
        }
    }

    // Draw border around the color display
    uint32_t border_color = 0xFFFFFFFF; // White border
    for (int x = 0; x < color_display_size; x++)
    {
        pixel_buffer_set(pb, display_x + x, display_y, border_color);
        pixel_buffer_set(pb, display_x + x, display_y + color_display_size - 1, border_color);
    }
    for (int y = 0; y < color_display_size; y++)
    {
        pixel_buffer_set(pb, display_x, display_y + y, border_color);
        pixel_buffer_set(pb, display_x + color_display_size - 1, display_y + y, border_color);
    }
}

// Utility function to clamp values
float clamp(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}
// Function to clamp a value between min and max
static float clamp_float(float value, float min, float max)
{
    if (value < min)
        return min;
    if (value > max)
        return max;
    return value;
}
// Function to handle color picker and brightness slider input
void handle_color_picker_input(State *state, PixelBuffer *pb, Vec2 mouse_pos, uint32_t mouse_buttons)
{
    // Define hue-saturation picker position
    int picker_x = pb->width - COLOR_PICKER_SIZE - COLOR_PICKER_MARGIN;
    int picker_y = COLOR_PICKER_MARGIN;

    // Define brightness slider position
    int slider_x = pb->width - BRIGHTNESS_SLIDER_WIDTH - BRIGHTNESS_SLIDER_MARGIN;
    int slider_y = BRIGHTNESS_SLIDER_MARGIN + COLOR_PICKER_SIZE + 10; // 10 pixels below color picker

    // Check if mouse is within the hue-saturation picker
    bool within_picker = (mouse_pos.x >= picker_x) &&
                         (mouse_pos.x < picker_x + COLOR_PICKER_SIZE) &&
                         (mouse_pos.y >= picker_y) &&
                         (mouse_pos.y < picker_y + COLOR_PICKER_SIZE);

    // Check if mouse is within the brightness slider
    bool within_slider = (mouse_pos.x >= slider_x) &&
                         (mouse_pos.x < slider_x + BRIGHTNESS_SLIDER_WIDTH) &&
                         (mouse_pos.y >= slider_y) &&
                         (mouse_pos.y < slider_y + BRIGHTNESS_SLIDER_HEIGHT);

    // Handle hue-saturation picker input
    if (within_picker && (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        // Calculate hue based on x position
        float relative_x = clamp_float((float)(mouse_pos.x - picker_x), 0.0f, (float)(COLOR_PICKER_SIZE - 1));
        float hue = (relative_x / (float)(COLOR_PICKER_SIZE - 1)) * 360.0f;

        // Calculate saturation based on y position (inverted, as y increases downward)
        float relative_y = clamp_float((float)(mouse_pos.y - picker_y), 0.0f, (float)(COLOR_PICKER_SIZE - 1));
        float saturation = 1.0f - (relative_y / (float)(COLOR_PICKER_SIZE - 1));

        // Update state hue and saturation
        state->hue = hue;
        state->saturation = saturation;

        // Update state->color based on new hue and saturation, keeping brightness
        uint8_t r, g, b;
        hsv_to_rgb(state->hue, state->saturation, state->brightness, &r, &g, &b);
        state->color = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFF; // RGBA

        // Optional: Print the updated HSV and RGB values
        printf("Selected Color: 0x%08X (H: %.2f°, S: %.2f, V: %.2f)\n", state->color, state->hue, state->saturation, state->brightness);
    }

    // Handle brightness slider input
    if (within_slider && (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)))
    {
        // Calculate brightness based on x position
        float relative_x = clamp_float((float)(mouse_pos.x - slider_x), 0.0f, (float)(BRIGHTNESS_SLIDER_WIDTH - 1));
        float brightness = 1.0f - (relative_x / (float)(BRIGHTNESS_SLIDER_WIDTH - 1));

        // Update state brightness
        state->brightness = brightness;

        // Update state->color based on current hue, saturation, and new brightness
        uint8_t r, g, b;
        hsv_to_rgb(state->hue, state->saturation, state->brightness, &r, &g, &b);
        state->color = ((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 0xFF; // RGBA

        // Optional: Print the updated HSV and RGB values
        printf("Selected Color: 0x%08X (H: %.2f°, S: %.2f, V: %.2f)\n", state->color, state->hue, state->saturation, state->brightness);
    }
}

void draw(PixelBuffer *pb, FTexture *z_buffer, State *state, Assets *assets)
{
    // make a face buffer
    PixelBuffer *face_buffer = pixel_buffer_new(pb->width, pb->height);
    // fill the face_buffer with uint32_t max
    pixel_buffer_fill(face_buffer, UINT32_MAX);

    // IVec2 screen_center = ivec2_create(pb->width / 2, pb->height / 2);
    draw_grid(pb, ivec2_create(0, 0), ivec2_create(pb->width - 1, pb->height - 1), 20, COLOR_GRAY_DARK);

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
    // float time = SDL_GetTicks() / 1000.0f;
    // float angle = time * 0.5f;
    float angle = 0.0f;

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
    Mesh *mesh = assets->jet_plane_mesh;
    int grid_size = 1;
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
                draw_mesh(pb, z_buffer, face_buffer, state, assets,
                          mesh->vertices, mesh->indices, mesh->colors,
                          lights, num_lights,
                          vec3_create(x_pos, y_pos, z_pos),   // position
                          vec3_create(0.0f, angle, 0.0f),     // rotation
                          vec3_create(scalef, scalef, scalef) // scale
                );
            }
        }
    }

    // draw a tiny gba at each light
    // float light_gba_scale = 3.0f;
    // for (int i = 0; i < num_lights; i++)
    // {
    //     draw_mesh(pb, z_buffer, face_buffer, state, assets,
    //               assets->gba_mesh->vertices, assets->gba_mesh->indices, assets->gba_mesh->colors,
    //               lights, num_lights,
    //               lights[i].pos,                                                 // position
    //               vec3_create(0.0f, 0.0f, 0.0f),                                 // rotation
    //               vec3_create(light_gba_scale, light_gba_scale, light_gba_scale) // scale
    //     );
    // }

    // free lights
    free(lights);

    //     // debug the z buffer
    // {
    //     // get min and max z
    //     float min_z = FLT_MAX;
    //     float max_z = -FLT_MAX;
    //     for (int i = 0; i < z_buffer->width * z_buffer->height; i++)
    //     {
    //         float z = z_buffer->data[i];
    //         if (z < min_z)
    //         {
    //             min_z = z;
    //         }
    //         if (z > max_z)
    //         {
    //             max_z = z;
    //         }
    //     }

    //     float mapscale = 4.0f;
    //     for (int y = 0; y < z_buffer->height; y += (int)mapscale)
    //     {
    //         for (int x = 0; x < z_buffer->width; x += (int)mapscale)
    //         {
    //             float z = f_texture_get(z_buffer, x, y);
    //             // z = map_range(z, min_z, max_z, 0.0f, 1.0f);
    //             // z = 1.0f - z;
    //             // high pow
    //             // z = pow(z, 1000.0f);
    //             z /= 500.0f;
    //             uint8_t z8 = (uint8_t)(z * 255.0f);
    //             uint32_t color = color_from_rgb(z8, z8, z8);
    //             set_pixel(pb, x / mapscale, y / mapscale, color);

    //             // if (z < 500.0f)
    //             // {
    //             //     // write out in text the z value on screen
    //             //     char z_str[10];
    //             //     sprintf(z_str, "%.1f", z);
    //             //     blit_string(pb, assets->charmap_white, z_str, x, y, 1, COLOR_WHITE);
    //             // }
    //         }
    //     }
    // }

    // debug the face buffer
    {
        // get min and max face
        uint32_t min_face = UINT32_MAX;
        uint32_t max_face = 0;
        for (int i = 0; i < face_buffer->width * face_buffer->height; i++)
        {
            uint32_t face = face_buffer->pixels[i];
            if (face < min_face)
            {
                min_face = face;
            }
            if (face > max_face)
            {
                max_face = face;
            }
        }

        float mapscale = 4.0f;
        for (int y = 0; y < face_buffer->height; y += (int)mapscale)
        {
            for (int x = 0; x < face_buffer->width; x += (int)mapscale)
            {
                uint32_t face = pixel_buffer_get(face_buffer, x, y);
                face = map_range(face, min_face, max_face, 100, 255);
                uint32_t color = color_from_rgb(face, face, face);
                pixel_buffer_set(pb, x / mapscale, y / mapscale, color);
            }
        }

        // just print the face buffer to terminal
        // for (int y = 0; y < face_buffer->height; y++)
        // {
        //     for (int x = 0; x < face_buffer->width; x++)
        //     {
        //         uint32_t face = get_pixel(face_buffer, x, y);
        //         printf("%d ", face);
        //     }
        //     printf("\n");
        // }
    }

    {
        // Draw the hue-saturation color picker
        draw_hue_saturation_picker(pb, state);
        draw_brightness_slider(pb, state);
        draw_selected_color(pb, state->color);

        // Handle color picker input
        MouseState mouseState;
        mouseState.buttons = get_mouse_buttons();
        mouseState.pos = get_mouse_pos();
        handle_color_picker_input(state, pb, ivec2_to_vec2(mouseState.pos), mouseState.buttons);
    }

    Vec2 mouse_pos = ivec2_to_vec2(get_mouse_pos());
    draw_cursor(pb, mouse_pos.x, mouse_pos.y, 10, 0xFF0000FF);

    // draw a rect at the cursor of given size
    const int cursor_size = 5;
    draw_rect(pb, mouse_pos.x - cursor_size / 2, mouse_pos.y - cursor_size / 2, cursor_size, cursor_size, color_set_alpha(state->color, 255));

    // if mouse is pressed, get the coord from the face buffer, set the color to red in the jet model colors list
    {
        MouseState mouseState;
        mouseState.buttons = get_mouse_buttons();
        mouseState.pos = get_mouse_pos();

        // picker
        int picker_x = pb->width - COLOR_PICKER_SIZE - COLOR_PICKER_MARGIN;
        int picker_y = COLOR_PICKER_MARGIN;
        if (is_left_mouse_button_down(&mouseState))
        {
            for (int y = mouseState.pos.y - cursor_size / 2; y < mouseState.pos.y + cursor_size / 2; y++)
            {
                for (int x = mouseState.pos.x - cursor_size / 2; x < mouseState.pos.x + cursor_size / 2; x++)
                {
                    // check if in picker
                    // Check if mouse is within the color picker area
                    bool within_picker = (x >= picker_x) &&
                                         (x < picker_x + COLOR_PICKER_SIZE) &&
                                         (y >= picker_y) &&
                                         (y < picker_y + COLOR_PICKER_SIZE);
                    if (!within_picker && x >= 0 && x < pb->width && y >= 0 && y < pb->height)
                    {

                        // get the face under the cursor
                        uint32_t face = pixel_buffer_get(face_buffer, x, y);
                        // skip if value is uint32_max
                        if (face != UINT32_MAX)
                        {
                            mesh->colors->data[face] = state->color;
                        }
                    }
                }
            }
        }
    }

    // cleanup
    pixel_buffer_free(face_buffer);
}
