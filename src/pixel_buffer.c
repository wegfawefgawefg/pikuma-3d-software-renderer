#include "pixel_buffer.h"
#include "utils.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

PixelBuffer *create_pixel_buffer(int width, int height)
{
    PixelBuffer *pb = (PixelBuffer *)malloc(sizeof(PixelBuffer));
    if (!pb)
    {
        return NULL;
    }

    pb->width = width;
    pb->height = height;
    pb->pixels = (uint32_t *)calloc(width * height, sizeof(uint32_t));
    if (!pb->pixels)
    {
        free(pb);
        return NULL;
    }

    return pb;
}

void destroy_pixel_buffer(PixelBuffer *pb)
{
    free(pb->pixels);
    free(pb);
}

void copy_to_texture(PixelBuffer *pb, SDL_Texture *texture)
{
    SDL_UpdateTexture(texture, NULL, pb->pixels, pb->width * sizeof(uint32_t));
}

void set_pixel(PixelBuffer *pb, int x, int y, uint32_t color)
{
    if (x < 0 || x >= pb->width || y < 0 || y >= pb->height)
    {
        return;
    }
    pb->pixels[y * pb->width + x] = color;
}

uint32_t get_pixel(PixelBuffer *pb, int x, int y)
{
    if (x < 0 || x >= pb->width || y < 0 || y >= pb->height)
    {
        return 0;
    }
    return pb->pixels[y * pb->width + x];
}

void clear_pixel_buffer(PixelBuffer *pb, uint32_t color)
{
    for (int i = 0; i < pb->width * pb->height; i++)
    {
        pb->pixels[i] = color;
    }
}

// just subtract amount from each color channel and alpha channel, colors are 4 bytes
void fade_pixel_buffer(PixelBuffer *pb, uint8_t amount)
{
    for (int i = 0; i < pb->width * pb->height; i++)
    {
        uint32_t color = pb->pixels[i];
        uint8_t *color_bytes = (uint8_t *)&color;
        for (int j = 0; j < 4; j++)
        {
            if (color_bytes[j] > amount)
            {
                color_bytes[j] -= amount;
            }
            else
            {
                color_bytes[j] = 0;
            }
        }
        pb->pixels[i] = color;
    }
}

PixelBuffer *scale_pixelbuffer(PixelBuffer *src, Vec2 scale)
{
    if (!src || scale.x <= 0 || scale.y <= 0)
        return NULL;

    int new_width = (int)(src->width * scale.x);
    int new_height = (int)(src->height * scale.y);

    PixelBuffer *scaled = create_pixel_buffer(new_width, new_height);
    if (!scaled)
        return NULL; // Handle allocation failure

    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            int src_x = (int)(x / scale.x);
            int src_y = (int)(y / scale.y);

            // Ensure we don't go out of bounds
            src_x = (src_x >= src->width) ? src->width - 1 : src_x;
            src_y = (src_y >= src->height) ? src->height - 1 : src_y;

            uint32_t color = get_pixel(src, src_x, src_y);
            set_pixel(scaled, x, y, color);
        }
    }

    return scaled;
}

PixelBuffer *rotate_pixelbuffer(PixelBuffer *src, float degrees)
{
    // Calculate the center of the source image
    float center_x = src->width / 2.0f;
    float center_y = src->height / 2.0f;

    // Convert degrees to radians
    float radians = degrees * (M_PI / 180.0f);
    float cos_r = cosf(radians);
    float sin_r = sinf(radians);

    // Calculate the dimensions of the rotated image
    int w = src->width;
    int h = src->height;
    int max_dim = (int)ceil(sqrtf(w * w + h * h)) + 1;

    // Create a new PixelBuffer for the rotated image
    PixelBuffer *rotated = create_pixel_buffer(max_dim, max_dim);
    if (!rotated)
        return NULL; // Handle allocation failure

    // Translate center to the middle of the new buffer
    float translate_x = max_dim / 2.0f - center_x;
    float translate_y = max_dim / 2.0f - center_y;

    for (int y = 0; y < max_dim; y++)
    {
        for (int x = 0; x < max_dim; x++)
        {
            // Translate point to rotate around specified center
            float dx = x - translate_x - center_x;
            float dy = y - translate_y - center_y;

            // Rotate the point
            int sx = (int)roundf((dx * cos_r + dy * sin_r) + center_x);
            int sy = (int)roundf((-dx * sin_r + dy * cos_r) + center_y);

            // Check if the source pixel is within the bounds of the source image
            if (sx >= 0 && sx < w && sy >= 0 && sy < h)
            {
                uint32_t color = get_pixel(src, sx, sy);
                set_pixel(rotated, x, y, color);
            }
            else
            {
                // Set to transparent or background color if out of bounds
                set_pixel(rotated, x, y, 0x00000000); // Assuming ARGB format
            }
        }
    }

    return rotated;
}

void blit(PixelBuffer *src, PixelBuffer *dst, int x, int y)
{
    // determine the intersection bounds
    int x0 = x;
    int x1 = x + src->width;
    int y0 = y;
    int y1 = y + src->height;
    if (x0 < 0)
        x0 = 0;
    if (x1 > dst->width)
        x1 = dst->width;
    if (y0 < 0)
        y0 = 0;
    if (y1 > dst->height)
        y1 = dst->height;

    for (int j = y0; j < y1; j++)
    {
        for (int i = x0; i < x1; i++)
        {
            uint32_t src_color = get_pixel(src, i - x, j - y);
            uint32_t dst_color = get_pixel(dst, i, j);

            // Extract color components and alpha, assume RGBA
            uint8_t src_r = (src_color >> 24) & 0xFF;
            uint8_t src_g = (src_color >> 16) & 0xFF;
            uint8_t src_b = (src_color >> 8) & 0xFF;
            uint8_t src_a = src_color & 0xFF;

            // If source alpha is max, just copy the source pixel
            if (src_a == 255)
            {
                set_pixel(dst, i, j, src_color);
            }
            else if (src_a > 0)
            { // Only blend if there's some opacity
                uint8_t dst_r = (dst_color >> 24) & 0xFF;
                uint8_t dst_g = (dst_color >> 16) & 0xFF;
                uint8_t dst_b = (dst_color >> 8) & 0xFF;
                uint8_t dst_a = dst_color & 0xFF;

                // Blend colors using integer arithmetic
                uint16_t r = (uint16_t)src_r * src_a + (uint16_t)dst_r * (255 - src_a);
                uint16_t g = (uint16_t)src_g * src_a + (uint16_t)dst_g * (255 - src_a);
                uint16_t b = (uint16_t)src_b * src_a + (uint16_t)dst_b * (255 - src_a);

                // Compute new alpha
                uint16_t a = (uint16_t)src_a + (uint16_t)dst_a * (255 - src_a) / 255;

                // Normalize results
                r = (r + 127) / 255;
                g = (g + 127) / 255;
                b = (b + 127) / 255;

                // Ensure we don't overflow
                uint8_t final_r = (uint8_t)(r > 255 ? 255 : r);
                uint8_t final_g = (uint8_t)(g > 255 ? 255 : g);
                uint8_t final_b = (uint8_t)(b > 255 ? 255 : b);
                uint8_t final_a = (uint8_t)(a > 255 ? 255 : a);

                // Combine the blended components
                uint32_t blended_color = (final_r << 24) | (final_g << 16) | (final_b << 8) | final_a;

                set_pixel(dst, i, j, blended_color);
            }
            // If src_a is 0, we don't need to do anything (keep destination pixel)
        }
    }
}

void blit_with_scale(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, Vec2 scale)
{
    if (!src || !dst || scale.x <= 0 || scale.y <= 0)
        return;

    // Create a temporary scaled buffer
    PixelBuffer *scaled = scale_pixelbuffer(src, scale);
    if (!scaled)
        return; // Handle allocation failure

    // Blit the scaled buffer onto the destination
    blit(scaled, dst, pos.x, pos.y);

    // Clean up
    destroy_pixel_buffer(scaled);
}

void blit_with_rotation(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, float angle, Vec2 center_of_rotation)
{
    // rotate the buffer around its center
    PixelBuffer *rotated = rotate_pixelbuffer(src, angle);

    Vec2 old_center = ivec2_to_vec2(get_center_of_pixelbuffer(src));
    Vec2 new_center = ivec2_to_vec2(get_center_of_pixelbuffer(rotated));

    Vec2 old_target = center_of_rotation;
    Vec2 new_target = vec2_rotate_point_around_pivot(old_target, old_center, angle);
    Vec2 center_to_new_target = vec2_sub(new_target, old_center);
    Vec2 rotated_pos = vec2_sub(ivec2_to_vec2(pos), new_center);
    rotated_pos = vec2_sub(rotated_pos, center_to_new_target);
    blit(rotated, dst, rotated_pos.x, rotated_pos.y);

    destroy_pixel_buffer(rotated);
}

void blit_with_scale_and_rotation(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, Vec2 scale, float angle, Vec2 center_of_rotation)
{
    if (!src || !dst || scale.x <= 0 || scale.y <= 0)
        return;

    // Scale first
    PixelBuffer *scaled = scale_pixelbuffer(src, scale);
    if (!scaled)
        return; // Handle allocation failure

    // Then rotate
    PixelBuffer *rotated = rotate_pixelbuffer(scaled, angle);
    destroy_pixel_buffer(scaled);
    if (!rotated)
        return; // Handle allocation failure

    // Calculate new position
    Vec2 old_center = ivec2_to_vec2(get_center_of_pixelbuffer(src));
    Vec2 new_center = ivec2_to_vec2(get_center_of_pixelbuffer(rotated));
    Vec2 old_target = center_of_rotation;
    Vec2 new_target = vec2_rotate_point_around_pivot(old_target, old_center, angle);
    Vec2 center_to_new_target = vec2_sub(new_target, old_center);
    Vec2 rotated_pos = vec2_sub(ivec2_to_vec2(pos), new_center);
    // without the scale, the position is correct, but we have to adjust the position now based on the scale
    center_to_new_target = vec2_mul(center_to_new_target, scale);
    Vec2 new_pos = vec2_sub(rotated_pos, center_to_new_target);

    // Blit the rotated and scaled buffer onto the destination
    blit(rotated, dst, new_pos.x, new_pos.y);

    // Clean up
    destroy_pixel_buffer(rotated);
}

// void blit_with_scale_and_rotation(PixelBuffer *src, PixelBuffer *dst, IVec2 pos, Vec2 scale, float angle, Vec2 pin)
// {
//     if (!src || !dst || scale.x <= 0 || scale.y <= 0)
//         return;

//     // Scale first
//     PixelBuffer *scaled = scale_pixelbuffer(src, scale);
//     if (!scaled)
//         return; // Handle allocation failure

//     // Then rotate
//     PixelBuffer *rotated = rotate_pixelbuffer(scaled, angle);
//     destroy_pixel_buffer(scaled);
//     if (!rotated)
//         return; // Handle allocation failure

//     // Calculate the position of the pin in the original image
//     Vec2 src_pin_offset = vec2_sub(pin, vec2_create(0, 0));

//     // Calculate the position of the pin after scaling
//     Vec2 scaled_pin_offset = vec2_mul(src_pin_offset, scale);

//     // Rotate the scaled pin offset
//     float cos_angle = cosf(angle * M_PI / 180.0f);
//     float sin_angle = sinf(angle * M_PI / 180.0f);
//     Vec2 rotated_pin_offset = {
//         scaled_pin_offset.x * cos_angle - scaled_pin_offset.y * sin_angle,
//         scaled_pin_offset.x * sin_angle + scaled_pin_offset.y * cos_angle};

//     // Calculate the final position adjustment
//     Vec2 position_adjustment = vec2_sub(src_pin_offset, rotated_pin_offset);

//     // Calculate the final blitting position
//     IVec2 final_pos = {
//         pos.x + (int)roundf(position_adjustment.x),
//         pos.y + (int)roundf(position_adjustment.y)};

//     // Blit the rotated and scaled buffer onto the destination
//     blit(rotated, dst, final_pos.x, final_pos.y);

//     // Clean up
//     destroy_pixel_buffer(rotated);
// }

void blit_dumb(PixelBuffer *src, PixelBuffer *dst, int x, int y)
{
    // determine the intersection bounds
    int x0 = x;
    int x1 = x + src->width;
    int y0 = y;
    int y1 = y + src->height;

    if (x0 < 0)
        x0 = 0;
    if (x1 > dst->width)
        x1 = dst->width;
    if (y0 < 0)
        y0 = 0;
    if (y1 > dst->height)
        y1 = dst->height;

    for (int j = y0; j < y1; j++)
    {
        for (int i = x0; i < x1; i++)
        {
            uint32_t src_color = get_pixel(src, i - x, j - y);
            set_pixel(dst, i, j, src_color);
        }
    }
}

void color_rotate(PixelBuffer *pb, float hue_shift)
{
    for (int i = 0; i < pb->width * pb->height; i++)
    {
        uint32_t color = pb->pixels[i];
        uint8_t r = (color >> 24) & 0xFF;
        uint8_t g = (color >> 16) & 0xFF;
        uint8_t b = (color >> 8) & 0xFF;
        uint8_t a = color & 0xFF;

        float h, s, v;
        rgb_to_hsv(r, g, b, &h, &s, &v);

        h += hue_shift;
        while (h >= 360.0f)
            h -= 360.0f;
        while (h < 0.0f)
            h += 360.0f;

        hsv_to_rgb(h, s, v, &r, &g, &b);

        pb->pixels[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }
}

PixelBuffer *load_pixelbuffer_from_png(const char *filename)
{
    int width, height, channels;
    // Load the image using stb_image, expecting 4 channels (RGBA)
    uint8_t *data = stbi_load(filename, &width, &height, &channels, 4);
    if (!data)
    {
        printf("Failed to load image: %s\n", stbi_failure_reason());
        return NULL;
    }

    // allocate memory for the pixel buffer
    PixelBuffer *buffer = create_pixel_buffer(width, height);
    if (!buffer)
    {
        stbi_image_free(data);
        return NULL;
    }

    // Manually set each pixel in RGBA order
    for (int i = 0; i < width * height; i++)
    {
        uint8_t r = data[i * 4];
        uint8_t g = data[i * 4 + 1];
        uint8_t b = data[i * 4 + 2];
        uint8_t a = data[i * 4 + 3];

        // Set pixel in RGBA order
        buffer->pixels[i] = (r << 24) | (g << 16) | (b << 8) | a;
    }

    stbi_image_free(data);
    return buffer;
}

IVec2 calculate_new_top_left(PixelBuffer *src, float degrees, Vec2 center_of_rotation)
{
    // Calculate the center of the source image
    float src_center_x = src->width / 2.0f;
    float src_center_y = src->height / 2.0f;

    // Convert degrees to radians
    float radians = degrees * (M_PI / 180.0f);
    float cos_r = cosf(radians);
    float sin_r = sinf(radians);

    // Calculate the rotated position of the center of rotation
    float dx = center_of_rotation.x - src_center_x;
    float dy = center_of_rotation.y - src_center_y;
    float rotated_x = dx * cos_r - dy * sin_r + src_center_x;
    float rotated_y = dx * sin_r + dy * cos_r + src_center_y;

    // Calculate the offset to keep the rotated center at the same position
    IVec2 new_topleft;
    new_topleft.x = (int)roundf(center_of_rotation.x - rotated_x);
    new_topleft.y = (int)roundf(center_of_rotation.y - rotated_y);

    return new_topleft;
}

// function to draw a red outline at the edges of the pixel buffer
void draw_outline(PixelBuffer *pb, uint32_t color)
{
    for (int i = 0; i < pb->width; i++)
    {
        set_pixel(pb, i, 0, color);
        set_pixel(pb, i, pb->height - 1, color);
    }
    for (int i = 0; i < pb->height; i++)
    {
        set_pixel(pb, 0, i, color);
        set_pixel(pb, pb->width - 1, i, color);
    }
}

// get center
IVec2 get_center_of_pixelbuffer(PixelBuffer *pb)
{
    return ivec2_create(pb->width / 2, pb->height / 2);
}