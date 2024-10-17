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

    // print info on the texture, like Loaded Texture: path: path, width: %d, height: %d
    printf("\033[0;32mLoaded Texture: \033[0mpath: %s, width: %d, height: %d\n", path, (*texture)->width, (*texture)->height);

    return (LoadResult){true, NULL};
}

LoadResult load_pixel_buffer(PixelBuffer **buffer, const char *path)
{
    *buffer = load_pixelbuffer_from_png(path);
    if (!*buffer)
    {
        return (LoadResult){false, path};
    }

    // print info on the pixel buffer, like Loaded PixelBuffer: path: path, width: %d, height: %d
    printf("\033[0;32mLoaded PixelBuffer: \033[0mpath: %s, width: %d, height: %d\n", path, (*buffer)->width, (*buffer)->height);
    return (LoadResult){true, NULL};
}

LoadResult load_mesh(Mesh **mesh, const char *path)
{
    *mesh = mesh_load_from_obj(path);
    if (!*mesh)
    {
        return (LoadResult){false, path};
    }

    // print info on the mesh, like Loaded Mesh: path: path, and the other lengths tabbed over on the next lines
    printf("\033[0;32mLoaded Mesh: \033[0mpath: %s,\n", path);
    printf("\tVertices: %d\n", (*mesh)->vertices->length / 3);
    printf("\tNormals: %d\n", (*mesh)->normals->length / 3);
    printf("\tTexcoords: %d\n", (*mesh)->texcoords->length / 2);
    printf("\tVertex Indices: %d\n", (*mesh)->vertex_indices->length);
    printf("\tNormal Indices: %d\n", (*mesh)->normal_indices->length);
    printf("\tTexcoord Indices: %d\n", (*mesh)->texcoord_indices->length);

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
    LOAD_PIXEL_BUFFER(gba_texture, "./assets/textures/gba_smol_crunchy.png")
    LOAD_MESH(gba_mesh, "assets/models/gba.obj")
    LOAD_PIXEL_BUFFER(charmap_white, "./assets/charmap_white.png")
    LOAD_MESH(jet_plane_mesh, "assets/models/f22.obj")
    LOAD_MESH(cube_mesh, "assets/models/cube.obj")

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
    if (assets->gba_texture)
        pixel_buffer_free(assets->gba_texture);
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

/*
    Load a mesh from an OBJ file.
    Also loads colors from a corresponding .col file.
*/
Mesh *mesh_load_from_obj(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open OBJ file: %s\n", filename);
        return NULL;
    }

    Mesh *mesh = mesh_new();
    if (!mesh)
    {
        fclose(file);
        return NULL;
    }

    // allocate
    char line[256];
    mesh->vertices = sfa_new(0);
    mesh->normals = sfa_new(0);
    mesh->texcoords = sfa_new(0);
    mesh->vertex_indices = su32a_new(0);
    mesh->normal_indices = su32a_new(0);
    mesh->texcoord_indices = su32a_new(0);
    mesh->colors = su32a_new(0);

    // check allocations
    if (!mesh->vertices || !mesh->normals || !mesh->texcoords ||
        !mesh->vertex_indices || !mesh->normal_indices || !mesh->texcoord_indices)
    {
        fprintf(stderr, "Failed to allocate temporary arrays for OBJ parsing\n");
        fclose(file);
        mesh_free(mesh);
        return NULL;
    }

    // Read the OBJ file line by line
    // v: vertex, vn: normal, vt: texcoord, f: face
    while (fgets(line, sizeof(line), file))
    {
        char *trimmed = trim_whitespace(line);
        // v: vertex
        if (trimmed[0] == 'v' && trimmed[1] == ' ')
        {
            // Vertex position
            float x, y, z;
            if (sscanf(trimmed, "v %f %f %f", &x, &y, &z) == 3)
            {
                // Append to vertices
                mesh->vertices->length += 3;
                float *new_data = realloc(mesh->vertices->data, sizeof(float) * mesh->vertices->length);
                if (!new_data)
                {
                    fprintf(stderr, "Failed to realloc vertices data\n");
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                mesh->vertices->data = new_data;
                const int latest_batch_start_index = mesh->vertices->length - 3;
                mesh->vertices->data[latest_batch_start_index] = x;
                mesh->vertices->data[latest_batch_start_index + 1] = y;
                mesh->vertices->data[latest_batch_start_index + 2] = z;
            }
            else
            {
                fprintf(stderr, "Invalid vertex format: %s\n", trimmed);
            }
        }
        // vn: normal
        else if (trimmed[0] == 'v' && trimmed[1] == 'n')
        {
            // Vertex normal
            float nx, ny, nz;
            if (sscanf(trimmed, "vn %f %f %f", &nx, &ny, &nz) == 3)
            {
                // Append to normals
                mesh->normals->length += 3;
                float *new_data = realloc(mesh->normals->data, sizeof(float) * mesh->normals->length);
                if (!new_data)
                {
                    fprintf(stderr, "Failed to realloc normals data\n");
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                mesh->normals->data = new_data;
                const int latest_batch_start_index = mesh->normals->length - 3;
                mesh->normals->data[latest_batch_start_index] = nx;
                mesh->normals->data[latest_batch_start_index + 1] = ny;
                mesh->normals->data[latest_batch_start_index + 2] = nz;
            }
            else
            {
                fprintf(stderr, "Invalid normal format: %s\n", trimmed);
            }
        }
        // vt: texcoord
        else if (trimmed[0] == 'v' && trimmed[1] == 't')
        {
            // Texture coordinate
            float u, v;
            if (sscanf(trimmed, "vt %f %f", &u, &v) == 2)
            {
                // Append to texcoords
                mesh->texcoords->length += 2;
                float *new_data = realloc(mesh->texcoords->data, sizeof(float) * mesh->texcoords->length);
                if (!new_data)
                {
                    fprintf(stderr, "Failed to realloc texcoords data\n");
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                mesh->texcoords->data = new_data;
                const int latest_batch_start_index = mesh->texcoords->length - 2;
                mesh->texcoords->data[latest_batch_start_index] = u;
                mesh->texcoords->data[latest_batch_start_index + 1] = v;
            }
            else
            {
                fprintf(stderr, "Invalid texture coordinate format: %s\n", trimmed);
            }
        }
        // f: face
        else if (trimmed[0] == 'f' && trimmed[1] == ' ')
        {
            // Face definition
            // OBJ faces can have different formats:
            // f v1 v2 v3
            // f v1/vt1 v2/vt2 v3/vt3
            // f v1//vn1 v2//vn2 v3//vn3
            // f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
            // We will only handle the last format

            int v0, v1, v2;
            int vt0, vt1, vt2;
            int vn0, vn1, vn2;
            // scanf the big one
            if (sscanf(trimmed, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                       &v0, &vt0, &vn0,
                       &v1, &vt1, &vn1,
                       &v2, &vt2, &vn2) == 9)
            {

                // Subtract 1 to convert from 1-based to 0-based indexing
                v0--;
                v1--;
                v2--;
                vt0--;
                vt1--;
                vt2--;
                vn0--;
                vn1--;
                vn2--;

                // Append to vertex_indices
                mesh->vertex_indices->length += 3;
                uint32_t *new_data_v = realloc(mesh->vertex_indices->data, sizeof(uint32_t) * mesh->vertex_indices->length);
                if (!new_data_v)
                {
                    fprintf(stderr, "Failed to realloc vertex_indices data\n");
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                mesh->vertex_indices->data = new_data_v;
                const int latest_batch_start_index_v = mesh->vertex_indices->length - 3;
                mesh->vertex_indices->data[latest_batch_start_index_v] = v0;
                mesh->vertex_indices->data[latest_batch_start_index_v + 1] = v1;
                mesh->vertex_indices->data[latest_batch_start_index_v + 2] = v2;

                // Append to texcoord_indices
                mesh->texcoord_indices->length += 3;
                uint32_t *new_data_vt = realloc(mesh->texcoord_indices->data, sizeof(uint32_t) * mesh->texcoord_indices->length);
                if (!new_data_vt)
                {
                    fprintf(stderr, "Failed to realloc texcoord_indices data\n");
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                mesh->texcoord_indices->data = new_data_vt;
                const int latest_batch_start_index_vt = mesh->texcoord_indices->length - 3;
                mesh->texcoord_indices->data[latest_batch_start_index_vt] = vt0;
                mesh->texcoord_indices->data[latest_batch_start_index_vt + 1] = vt1;
                mesh->texcoord_indices->data[latest_batch_start_index_vt + 2] = vt2;

                // Append to normal_indices
                mesh->normal_indices->length += 3;
                uint32_t *new_data_vn = realloc(mesh->normal_indices->data, sizeof(uint32_t) * mesh->normal_indices->length);
                if (!new_data_vn)
                {
                    fprintf(stderr, "Failed to realloc normal_indices data\n");
                    fclose(file);
                    mesh_free(mesh);
                    return NULL;
                }
                mesh->normal_indices->data = new_data_vn;
                const int latest_batch_start_index_vn = mesh->normal_indices->length - 3;
                mesh->normal_indices->data[latest_batch_start_index_vn] = vn0;
                mesh->normal_indices->data[latest_batch_start_index_vn + 1] = vn1;
                mesh->normal_indices->data[latest_batch_start_index_vn + 2] = vn2;
            }

            else
            {
                fprintf(stderr, "Invalid face format: %s\n", trimmed);
            }
        }
        else
        {
            // what is this line?
            // printf("Unknown line: %s\n", trimmed);
        }
    }

    fclose(file);

    // Load colors from the .col file as per your original code
    char col_filename[512];
    replace_extension_with_col(filename, col_filename, sizeof(col_filename));

    FILE *col_file = fopen(col_filename, "r");
    if (!col_file)
    {
        fprintf(stderr, "Failed to open color file: %s\n", col_filename);
        mesh->colors = NULL;
    }
    else
    {
        mesh->colors = su32a_new(0);
        if (!mesh->colors)
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
                mesh->colors->length += 1;
                uint32_t *new_data = realloc(mesh->colors->data, sizeof(uint32_t) * mesh->colors->length);
                if (!new_data)
                {
                    fprintf(stderr, "Failed to realloc colors data at line %d\n", line_number);
                    fclose(col_file);
                    mesh_free(mesh);
                    return NULL;
                }
                mesh->colors->data = new_data;
                const int last_color_index = mesh->colors->length - 1;
                mesh->colors->data[last_color_index] = packed_color;
            }
            else
            {
                fprintf(stderr, "Failed to parse color at line %d: %s", line_number, line);
                // Continue processing the next lines
            }
        }
        fclose(col_file);

        // Validate the number of colors against the number of faces
        if (mesh->vertex_indices->length == 0)
        {
            fprintf(stderr, "Mesh has no vertex indices (faces) to associate colors with.\n");
        }
        else
        {
            int expected_colors = mesh->vertex_indices->length / 3; // Assuming one color per triangle
            int actual_colors = mesh->colors->length;               // 1 uint32_t per color

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
    }

    // Additional validation can be performed here, such as ensuring that
    // the number of normals and texcoords match the number of vertices, etc.

    return mesh;
}