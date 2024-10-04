#include "assets.h"

#include <SDL2/SDL_image.h>

#include "utils.h"

SizedSDLTexture *load_sized_sdl_texture(const char *filename, SDL_Renderer *renderer)
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

void destroy_sized_sdl_texture(SizedSDLTexture *sized_sdl_texture)
{
    SDL_DestroyTexture(sized_sdl_texture->texture);
    free(sized_sdl_texture);
}

// typedef struct
// {
//     SizedSDLTexture *gba_overlay;
//     SizedSDLTexture *gba_power_light;
//     SizedSDLTexture *pointer_sized_sdl_texture;
//     PixelBuffer *pointer_pixel_buffer;
// } Assets;

Assets *load_assets(SDL_Renderer *renderer)
{
    Assets *assets = (Assets *)malloc(sizeof(Assets));

    assets->gba_overlay = load_sized_sdl_texture("./assets/textures/Clear-Purple-Shell-Game-Boy-Advance.png", renderer);
    if (!assets->gba_overlay)
    {
        printf("Failed to load gba overlay\n");
        return NULL;
    }

    assets->gba_power_light = load_sized_sdl_texture("./assets/textures/gbalight.png", renderer);
    if (!assets->gba_power_light)
    {
        printf("Failed to load gba power light\n");
        return NULL;
    }

    assets->pointer_sized_sdl_texture = load_sized_sdl_texture("./assets/textures/pointer.png", renderer);
    if (!assets->pointer_sized_sdl_texture)
    {
        printf("Failed to load pointer\n");
        return NULL;
    }

    assets->pointer_pixel_buffer = load_pixelbuffer_from_png("./assets/textures/pointer.png");
    if (!assets->pointer_pixel_buffer)
    {
        printf("Failed to load pointer into pixel buffer\n");
        return NULL;
    }

    // Load the OBJ model into the Mesh structure
    assets->gba_mesh = load_obj_to_mesh("assets/models/gba.obj");
    if (!assets->gba_mesh)
    {
        fprintf(stderr, "Failed to load gba.obj model\n");
        // Handle error as needed
    }

    // print out how many vertices and indices are in the gba mesh,
    // colors is the number of faces

    printf("gba mesh has %d vertices, %d indices, %d faces, %d colors\n",
           assets->gba_mesh->vertices->length / 3,
           assets->gba_mesh->indices->length,
           assets->gba_mesh->indices->length / 3,
           assets->gba_mesh->colors->length);

    // load the charmap
    assets->charmap_white = load_pixelbuffer_from_png("./assets/charmap_white.png");
    if (!assets->charmap_white)
    {
        printf("Failed to load charmap_white\n");
        return NULL;
    }

    return assets;
}

void destroy_assets(Assets *assets)
{
    destroy_sized_sdl_texture(assets->gba_overlay);
    destroy_sized_sdl_texture(assets->gba_power_light);
    destroy_sized_sdl_texture(assets->pointer_sized_sdl_texture);
    destroy_pixel_buffer(assets->pointer_pixel_buffer);
    destroy_pixel_buffer(assets->charmap_white);
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
Mesh *load_obj_to_mesh(const char *filename)
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
    SIA *indices = sia_new(0);
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
                    sia_free(indices);
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
                int *new_data = realloc(indices->data, sizeof(int) * indices->length);
                if (!new_data)
                {
                    fprintf(stderr, "Failed to realloc indices data\n");
                    sfa_free(vertices);
                    sia_free(indices);
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
