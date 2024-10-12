#include "assets.h"

#include <SDL2/SDL_image.h>

#include "utils.h"

SizedSDLTexture *sized_sdl_texture_load(const char *filename, SDL_Renderer *renderer)
{
    SDL_Surface *surface = IMG_Load(filename);
    if (!surface)
    {
        printf("Failed to load image: %s\n", IMG_GetError());
        return NULL;
    }

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    SizedSDLTexture *sized_sdl_texture = (SizedSDLTexture *)malloc(sizeof(SizedSDLTexture));
    sized_sdl_texture->texture = texture;
    sized_sdl_texture->width = surface->w;
    sized_sdl_texture->height = surface->h;

    SDL_FreeSurface(surface);

    return sized_sdl_texture;
}

void sized_sdl_texture_free(SizedSDLTexture *sized_sdl_texture)
{
    SDL_DestroyTexture(sized_sdl_texture->texture);
    free(sized_sdl_texture);
}

#include <stdbool.h>

typedef struct
{
    bool success;
    const char *error_message;
} LoadResult;

LoadResult load_texture(SizedSDLTexture **texture, const char *path, SDL_Renderer *renderer)
{
    *texture = sized_sdl_texture_load(path, renderer);
    if (!*texture)
    {
        return (LoadResult){false, path};
    }
    return (LoadResult){true, NULL};
}

LoadResult load_pixel_buffer(PixelBuffer **buffer, const char *path)
{
    *buffer = load_pixelbuffer_from_png(path);
    if (!*buffer)
    {
        return (LoadResult){false, path};
    }
    return (LoadResult){true, NULL};
}

LoadResult load_mesh(Mesh **mesh, const char *path)
{
    *mesh = mesh_load_from_obj(path);
    if (!*mesh)
    {
        return (LoadResult){false, path};
    }
    return (LoadResult){true, NULL};
}

Assets *load_assets(SDL_Renderer *renderer)
{
    Assets *assets = (Assets *)malloc(sizeof(Assets));
    if (!assets)
    {
        fprintf(stderr, "Failed to allocate memory for assets\n");
        return NULL;
    }

    LoadResult result;
    const char *error_message = NULL;

#define LOAD_TEXTURE(resource, path)                          \
    result = load_texture(&assets->resource, path, renderer); \
    if (!result.success)                                      \
    {                                                         \
        error_message = result.error_message;                 \
        goto cleanup;                                         \
    }

#define LOAD_PIXEL_BUFFER(resource, path)                \
    result = load_pixel_buffer(&assets->resource, path); \
    if (!result.success)                                 \
    {                                                    \
        error_message = result.error_message;            \
        goto cleanup;                                    \
    }

#define LOAD_MESH(resource, path)                \
    result = load_mesh(&assets->resource, path); \
    if (!result.success)                         \
    {                                            \
        error_message = result.error_message;    \
        goto cleanup;                            \
    }

    LOAD_TEXTURE(gba_overlay, "./assets/textures/Clear-Purple-Shell-Game-Boy-Advance.png")
    LOAD_TEXTURE(gba_power_light, "./assets/textures/gbalight.png")
    LOAD_TEXTURE(pointer_sized_sdl_texture, "./assets/textures/pointer.png")
    LOAD_PIXEL_BUFFER(pointer_pixel_buffer, "./assets/textures/pointer.png")
    LOAD_MESH(gba_mesh, "assets/models/gba.obj")
    LOAD_PIXEL_BUFFER(charmap_white, "./assets/charmap_white.png")
    LOAD_MESH(jet_plane_mesh, "assets/models/f22.obj")
    LOAD_MESH(cube_mesh, "assets/models/cube.obj")

    // Print mesh info
    printf("gba mesh has %d vertices, %d indices, %d faces, %d colors\n",
           assets->gba_mesh->vertices->length / 3,
           assets->gba_mesh->indices->length,
           assets->gba_mesh->indices->length / 3,
           assets->gba_mesh->colors->length);

    return assets;

cleanup:
    fprintf(stderr, "Failed to load %s\n", error_message);
    // Free all allocated resources
    if (assets->gba_overlay)
        sized_sdl_texture_free(assets->gba_overlay);
    if (assets->gba_power_light)
        sized_sdl_texture_free(assets->gba_power_light);
    if (assets->pointer_sized_sdl_texture)
        sized_sdl_texture_free(assets->pointer_sized_sdl_texture);
    if (assets->pointer_pixel_buffer)
        pixel_buffer_free(assets->pointer_pixel_buffer);
    if (assets->gba_mesh)
        mesh_free(assets->gba_mesh);
    if (assets->charmap_white)
        pixel_buffer_free(assets->charmap_white);
    if (assets->jet_plane_mesh)
        mesh_free(assets->jet_plane_mesh);
    if (assets->cube_mesh)
        mesh_free(assets->cube_mesh);
    free(assets);
    return NULL;
}

void destroy_assets(Assets *assets)
{
    sized_sdl_texture_free(assets->gba_overlay);
    sized_sdl_texture_free(assets->gba_power_light);
    sized_sdl_texture_free(assets->pointer_sized_sdl_texture);
    pixel_buffer_free(assets->pointer_pixel_buffer);
    pixel_buffer_free(assets->charmap_white);
    mesh_free(assets->gba_mesh);
    free(assets);
}

void replace_extension_with_col(const char *filename, char *col_filename, size_t size)
{
    // Find the last occurrence of '.' in the filename
    const char *dot = strrchr(filename, '.');
    if (dot)
    {
        // Calculate the length up to the last '.'
        size_t basename_length = dot - filename;
        // Ensure we don't exceed the buffer size
        if (basename_length < size)
        {
            strncpy(col_filename, filename, basename_length);
            col_filename[basename_length] = '\0'; // Null-terminate
        }
        else
        {
            // If basename is too long, truncate
            strncpy(col_filename, filename, size - 1);
            col_filename[size - 1] = '\0';
        }
    }
    else
    {
        // If there's no '.', copy the entire filename
        strncpy(col_filename, filename, size - 1);
        col_filename[size - 1] = '\0';
    }

    // Append ".col"
    strncat(col_filename, ".col", size - strlen(col_filename) - 1);
}

// Function to load the OBJ model into a Mesh structure
Mesh *mesh_load_from_obj(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open OBJ file: %s\n", filename);
        return NULL;
    }

    Mesh *mesh = mesh_new(0, 0);
    if (!mesh)
    {
        fclose(file);
        return NULL;
    }

    char line[256];
    // Temporary storage for vertices and indices
    SFA *vertices = sfa_new(0);
    SU32A *indices = su32a_new(0);
    if (!vertices || !indices)
    {
        fprintf(stderr, "Failed to allocate temporary arrays for OBJ parsing\n");
        fclose(file);
        mesh_free(mesh);
        return NULL;
    }

    // Read the OBJ file line by line
    while (fgets(line, sizeof(line), file))
    {
        char *trimmed = trim_whitespace(line);
        if (trimmed[0] == 'v' && trimmed[1] == ' ')
        {
            // Vertex position
            float x, y, z;
            if (sscanf(trimmed, "v %f %f %f", &x, &y, &z) == 3)
            {
                // Append to vertices
                int current_length = vertices->length;
                vertices->length += 3;
                float *new_data = realloc(vertices->data, sizeof(float) * vertices->length);
                if (!new_data)
                {
                    fprintf(stderr, "Failed to realloc vertices data\n");
                    sfa_free(vertices);
                    su32a_free(indices);
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                vertices->data = new_data;
                vertices->data[current_length] = x;
                vertices->data[current_length + 1] = y;
                vertices->data[current_length + 2] = z;
            }
            else
            {
                fprintf(stderr, "Invalid vertex format: %s\n", trimmed);
            }
        }
        else if (trimmed[0] == 'f' && trimmed[1] == ' ')
        {
            // Face definition
            unsigned int v_indices[4] = {0};
            int vertex_count = 0;

            // Pointer to the face line, skip 'f '
            char *ptr = trimmed + 2;
            char *token;

            // Tokenize the face line by spaces
            while ((token = strtok_r(ptr, " ", &ptr)) != NULL)
            {
                unsigned int v_idx = 0;
                // Extract the vertex index before the first '/'
                if (sscanf(token, "%u", &v_idx) == 1)
                {
                    // Vertex index only
                }
                else if (sscanf(token, "%u/%*s", &v_idx) == 1)
                {
                    // Vertex index with slashes
                }
                else
                {
                    fprintf(stderr, "Invalid face format: %s\n", token);
                    continue;
                }

                // Subtract 1 to convert from 1-based to 0-based indexing
                if (v_idx == 0)
                {
                    fprintf(stderr, "Face vertex index is zero, which is invalid: %s\n", token);
                    continue;
                }

                v_indices[vertex_count++] = v_idx - 1;

                // Check if we have more than 3 vertices (e.g., quads)
                if (vertex_count >= 4)
                {
                    fprintf(stderr, "Warning: Face has more than 3 vertices, triangulating\n");
                }
            }

            // Triangulate the face if necessary
            for (int i = 1; i < vertex_count - 1; ++i)
            {
                int current_length = indices->length;
                indices->length += 3;
                uint32_t *new_data = realloc(indices->data, sizeof(uint32_t) * indices->length);
                if (!new_data)
                {
                    fprintf(stderr, "Failed to realloc indices data\n");
                    sfa_free(vertices);
                    su32a_free(indices);
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                indices->data = new_data;
                indices->data[current_length] = v_indices[0];
                indices->data[current_length + 1] = v_indices[i];
                indices->data[current_length + 2] = v_indices[i + 1];
            }
        }
        // You can extend this to handle other prefixes like 'vn', 'vt', etc.
    }

    fclose(file);

    // Assign parsed data to Mesh
    mesh->vertices = vertices;
    mesh->indices = indices;

    // colors are loaded from the .col file
    // each color is a single packed uint32_t
    /*
        ex:
        255 0 0 255
        0 255 0 255
        0 0 255 255
        and so on
    */

    // Load the colors from the .col file
    // File name is same as .col. For example: gba.obj has a gba.col file
    char col_filename[512];
    replace_extension_with_col(filename, col_filename, sizeof(col_filename));

    FILE *col_file = fopen(col_filename, "r");
    if (!col_file)
    {
        fprintf(stderr, "Failed to open color file: %s\n", col_filename);
        // Depending on your requirements, you might want to free resources here
        // For now, we'll proceed without colors
        mesh->colors = NULL;
        return mesh;
    }

    SU32A *colors = su32a_new(0);
    if (!colors)
    {
        fprintf(stderr, "Failed to allocate colors array\n");
        fclose(col_file);
        mesh_free(mesh);
        return NULL;
    }

    int line_number = 0;
    while (fgets(line, sizeof(line), col_file))
    {
        line_number++;
        uint8_t r, g, b, a;
        if (sscanf(line, "%hhu %hhu %hhu %hhu", &r, &g, &b, &a) == 4)
        {
            // Pack RGBA into a single uint32_t (e.g., ARGB format)
            uint32_t packed_color = ((uint32_t)r << 24) |
                                    ((uint32_t)g << 16) |
                                    ((uint32_t)b << 8) |
                                    ((uint32_t)a);

            // Append to colors
            int current_length = colors->length;
            colors->length += 1;
            uint32_t *new_data = realloc(colors->data, sizeof(uint32_t) * colors->length);
            if (!new_data)
            {
                fprintf(stderr, "Failed to realloc colors data at line %d\n", line_number);
                su32a_free(colors);
                fclose(col_file);
                mesh_free(mesh);
                return NULL;
            }
            colors->data = new_data;
            colors->data[current_length] = packed_color;
        }
        else
        {
            fprintf(stderr, "Failed to parse color at line %d: %s", line_number, line);
            // Continue processing the next lines
        }
    }

    fclose(col_file);

    mesh->colors = colors;

    // Validate the number of colors against the number of faces
    if (mesh->indices->length == 0)
    {
        fprintf(stderr, "Mesh has no indices (faces) to associate colors with.\n");
    }
    else
    {
        int expected_colors = mesh->indices->length / 3; // Assuming one color per triangle
        int actual_colors = (colors->length);            // 1 uint32_t per color

        if (actual_colors < expected_colors)
        {
            fprintf(stderr, "Not enough colors: expected %d, got %d\n", expected_colors, actual_colors);
        }
        else if (actual_colors > expected_colors)
        {
            fprintf(stderr, "Too many colors: expected %d, got %d\n", expected_colors, actual_colors);
        }
        else
        {
            printf("Color count matches face count: %d colors for %d faces.\n", actual_colors, expected_colors);
        }
    }

    return mesh;
}
