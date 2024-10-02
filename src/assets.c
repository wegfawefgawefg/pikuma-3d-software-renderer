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

    return assets;
}

void destroy_assets(Assets *assets)
{
    destroy_sized_sdl_texture(assets->gba_overlay);
    destroy_sized_sdl_texture(assets->gba_power_light);
    destroy_sized_sdl_texture(assets->pointer_sized_sdl_texture);
    destroy_pixel_buffer(assets->pointer_pixel_buffer);
    free(assets);
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
                vertices->data = realloc(vertices->data, sizeof(float) * vertices->length);
                if (!vertices->data)
                {
                    fprintf(stderr, "Failed to realloc vertices data\n");
                    sfa_free(vertices);
                    sia_free(indices);
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                vertices->data[current_length] = x;
                vertices->data[current_length + 1] = y;
                vertices->data[current_length + 2] = z;
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
                indices->data = realloc(indices->data, sizeof(int) * indices->length);
                if (!indices->data)
                {
                    fprintf(stderr, "Failed to realloc indices data\n");
                    sfa_free(vertices);
                    sia_free(indices);
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
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

    return mesh;
}
