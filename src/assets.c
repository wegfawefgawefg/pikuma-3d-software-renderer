#define _DEFAULT_SOURCE // Ensure feature test macros are set

#include "assets.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>   // For directory traversal
#include <sys/stat.h> // For file information
#include <errno.h>    // For error handling

#include "utils.h"
#include "pixel_buffer.h"

SizedSDLTexture *sized_sdl_texture_load(const char *filename, SDL_Renderer *renderer)
{
    char path[512];
    snprintf(path, sizeof(path), "./assets/sdl_textures/%s", filename);

    SDL_Surface *surface = IMG_Load(path);
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
    printf("\033[0;32mLoaded Texture: \033[0mfile: %s, width: %d, height: %d\n", path, (*texture)->width, (*texture)->height);

    return (LoadResult){true, NULL};
}

LoadResult load_pixel_buffer(PixelBuffer **buffer, const char *path)
{
    *buffer = pixelbuffer_load_from_png(path);
    if (!*buffer)
    {
        return (LoadResult){false, path};
    }

    // print info on the pixel buffer, like Loaded PixelBuffer: path: path, width: %d, height: %d
    printf("\033[0;32mLoaded PixelBuffer: \033[0mfile: %s, width: %d, height: %d\n", path, (*buffer)->width, (*buffer)->height);
    return (LoadResult){true, NULL};
}

LoadResult load_multi_frame_pixel_buffer(MultiFramePixelBuffer **mfpb, const char *path)
{
    *mfpb = mfpb_load_from_gif(path);
    if (!*mfpb)
    {
        return (LoadResult){false, path};
    }

    // print info on the multi frame pixel buffer, like Loaded MultiFramePixelBuffer: path: path, num_frames: %d, width: %d, height: %d
    printf("\033[0;32mLoaded MultiFramePixelBuffer: \033[0mfile: %s, num_frames: %d, width: %d, height: %d\n", path, (*mfpb)->num_frames, (*mfpb)->frames[0]->width, (*mfpb)->frames[0]->height);
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
    printf("\033[0;32mLoaded Mesh: \033[0mfile: %s,\n", path);
    printf("\tVertices: %d\n", (*mesh)->vertices->length / 3);
    printf("\tNormals: %d\n", (*mesh)->normals->length / 3);
    printf("\tTexcoords: %d\n", (*mesh)->texcoords->length / 2);
    printf("\tVertex Indices: %d\n", (*mesh)->vertex_indices->length);
    printf("\tNormal Indices: %d\n", (*mesh)->normal_indices->length);
    printf("\tTexcoord Indices: %d\n", (*mesh)->texcoord_indices->length);

    return (LoadResult){true, NULL};
}

// Helper function to append formatted error messages to a buffer
static void append_error(char *buffer, size_t size, size_t *offset, const char *format, ...)
{
    if (*offset >= size - 1)
        return; // Buffer is full

    va_list args;
    va_start(args, format);
    int written = vsnprintf(buffer + *offset, size - *offset, format, args);
    va_end(args);

    if (written > 0)
    {
        if ((size_t)written < size - *offset)
            *offset += written;
        else
            *offset = size - 1; // Prevent overflow
    }
}

#define LOAD_ASSET(load_func, asset_ptr, ...)                                       \
    do                                                                              \
    {                                                                               \
        LoadResult res = load_func(asset_ptr, __VA_ARGS__);                         \
        if (!res.success)                                                           \
        {                                                                           \
            append_error(error_buffer, sizeof(error_buffer), &error_offset,         \
                         "Failed to load %s: %s\n", #asset_ptr, res.error_message); \
            failed = true;                                                          \
        }                                                                           \
    } while (0)

Assets *assets_new(void)
{
    Assets *assets = (Assets *)calloc(1, sizeof(Assets));
    if (!assets)
    {
        fprintf(stderr, "Failed to allocate memory for assets\n");
        return NULL;
    }

    // set all your pointers to null
    assets->gba_mesh = NULL;
    assets->cube_mesh = NULL;
    assets->quad_mesh = NULL;
    assets->triangle_mesh = NULL;
    assets->textures = NULL;
    assets->earth_mfpb = NULL;

    return assets;
}

Assets *assets_load(void)
{
    Assets *assets = (Assets *)calloc(1, sizeof(Assets));
    if (!assets)
    {
        fprintf(stderr, "Failed to allocate memory for assets\n");
        return NULL;
    }

    // Buffer to accumulate error messages
    char error_buffer[1024] = {0};
    size_t error_offset = 0;
    bool failed = false;

    LOAD_ASSET(load_mesh, &assets->gba_mesh, "gba.obj");
    LOAD_ASSET(load_mesh, &assets->cube_mesh, "cube.obj");
    LOAD_ASSET(load_mesh, &assets->quad_mesh, "quad.obj");
    LOAD_ASSET(load_mesh, &assets->triangle_mesh, "triangle.obj");

    LOAD_ASSET(load_multi_frame_pixel_buffer, &assets->earth_mfpb, "earth.gif");

    // Check if any asset failed to load
    if (failed)
    {
        fprintf(stderr, "%s", error_buffer);
        assets_free(assets);
        return NULL;
    }

    // Load all PNG textures from the specified directory
    const char *texture_directory = "./assets/textures/";
    Textures *textures = textures_new();
    if (textures_load_from_directory(textures, texture_directory) != 0)
    {
        fprintf(stderr, "Failed to load textures from directory: %s\n", texture_directory);
        // Depending on your application's requirements, you might choose to exit or continue
    }
    assets->textures = textures;

    // All assets loaded successfully
    return assets;
}

#undef LOAD_ASSET

void assets_free(Assets *assets)
{
    if (!assets)
    {
        return;
    }

    mesh_free(assets->gba_mesh);
    mesh_free(assets->cube_mesh);
    mesh_free(assets->quad_mesh);
    mesh_free(assets->triangle_mesh);

    mfpb_free(assets->earth_mfpb);

    textures_free(assets->textures);

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
    char path[512];
    snprintf(path, sizeof(path), "./assets/models/%s", filename);

    FILE *file = fopen(path, "r");
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
            // that format is vertex/texcoord/normal indices

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

    // Load colors from the .col file
    char col_filename[512];
    replace_extension_with_col(filename, col_filename, sizeof(col_filename));

    // colorfile path
    char col_path[512];
    snprintf(col_path, sizeof(col_path), "./assets/models/%s", col_filename);

    FILE *col_file = fopen(col_path, "r");
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

////////////////////////////////////////////////////////////////////////////////
// Textures Management
////////////////////////////////////////////////////////////////////////////////
Textures *textures_new(void)
{
    Textures *textures = (Textures *)malloc(sizeof(Textures));
    if (!textures)
    {
        fprintf(stderr, "Failed to allocate memory for Textures.\n");
        return NULL;
    }
    textures->entries = NULL;
    textures->last = NULL;
    textures->num_entries = 0;
    return textures;
}

void textures_free(Textures *textures)
{
    if (!textures)
        return;

    TexturesEntry *current = textures->entries;
    while (current)
    {
        TexturesEntry *next = current->next;

        // Free the path and filename strings
        if (current->path)
            free(current->path);
        if (current->filename)
            free(current->filename);

        // Free the PixelBuffer
        if (current->texture)
            pixel_buffer_free(current->texture);

        // Free the entry itself
        free(current);

        current = next;
    }

    // Finally, free the Textures struct
    free(textures);
}

// Add a new texture to the Textures struct
// Returns 0 on success, -1 on failure
int textures_add(Textures *textures, const char *path, const char *filename)
{
    if (!textures || !path || !filename)
    {
        fprintf(stderr, "Invalid arguments to textures_add.\n");
        return -1;
    }

    // Allocate memory for the new entry
    TexturesEntry *new_entry = (TexturesEntry *)malloc(sizeof(TexturesEntry));
    if (!new_entry)
    {
        fprintf(stderr, "Failed to allocate memory for TexturesEntry.\n");
        return -1;
    }

    // Duplicate the path and filename strings
    new_entry->path = strdup(path);
    if (!new_entry->path)
    {
        fprintf(stderr, "Failed to duplicate path string.\n");
        free(new_entry);
        return -1;
    }

    new_entry->filename = strdup(filename);
    if (!new_entry->filename)
    {
        fprintf(stderr, "Failed to duplicate filename string.\n");
        free(new_entry->path);
        free(new_entry);
        return -1;
    }

    // Construct the full path for loading the texture
    size_t path_len = strlen(path);
    int needs_slash = (path[path_len - 1] != '/');
    size_t full_path_length = path_len + (needs_slash ? 1 : 0) + strlen(filename) + 1; // '/' + filename + '\0'
    char *full_path = (char *)malloc(full_path_length);
    if (!full_path)
    {
        fprintf(stderr, "Failed to allocate memory for full path.\n");
        free(new_entry->path);
        free(new_entry->filename);
        free(new_entry);
        return -1;
    }

    // Concatenate path and filename with '/' if necessary
    if (needs_slash)
    {
        snprintf(full_path, full_path_length, "%s/%s", path, filename);
    }
    else
    {
        snprintf(full_path, full_path_length, "%s%s", path, filename);
    }

    // Load the texture using pixelbuffer_load_from_png
    new_entry->texture = pixelbuffer_load_from_png(full_path);
    if (!new_entry->texture)
    {
        fprintf(stderr, "Failed to load texture from %s.\n", full_path);
        free(full_path);
        free(new_entry->path);
        free(new_entry->filename);
        free(new_entry);
        return -1;
    }

    free(full_path);

    // Initialize the next pointer
    new_entry->next = NULL;

    // Add the new entry to the linked list
    if (!textures->entries)
    {
        // First entry in the list
        textures->entries = new_entry;
        textures->last = new_entry;
    }
    else
    {
        // Append to the end and update the last pointer
        textures->last->next = new_entry;
        textures->last = new_entry;
    }

    textures->num_entries += 1;

    return 0;
}

PixelBuffer *textures_get(Textures *textures, const char *filename)
{
    if (!textures || !filename)
        return NULL;

    TexturesEntry *current = textures->entries;
    while (current)
    {
        if (strcmp(current->filename, filename) == 0)
        {
            return current->texture;
        }
        current = current->next;
    }

    // If not found, return NULL
    return NULL;
}

void textures_print(Textures *textures)
{
    if (!textures)
    {
        printf("Textures is NULL.\n");
        return;
    }

    printf("Total Textures: %d\n", textures->num_entries);
    printf("Textures List:\n");
    printf("----------------------------\n");

    TexturesEntry *current = textures->entries;
    int index = 1;
    while (current)
    {
        printf("Texture %d:\n", index);
        printf("  Path: %s\n", current->path);
        printf("  Filename: %s\n", current->filename);
        printf("  Dimensions: %dx%d\n", current->texture->width, current->texture->height);
        printf("----------------------------\n");
        current = current->next;
        index++;
    }
}

static int has_png_extension(const char *filename)
{
    if (!filename)
        return 0;
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename)
        return 0;
    return (strcasecmp(dot, ".png") == 0); // Case-insensitive comparison
}

int textures_load_from_directory(Textures *textures, const char *directory_path)
{
    if (!textures || !directory_path)
    {
        fprintf(stderr, "Invalid arguments to textures_load_from_directory.\n");
        return -1;
    }

    DIR *dir;
    struct dirent *entry;

    dir = opendir(directory_path);
    if (!dir)
    {
        perror("opendir");
        fprintf(stderr, "Failed to open directory: %s\n", directory_path);
        return -1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip directories
        if (entry->d_type == DT_DIR)
            continue;

        const char *filename = entry->d_name;

        if (has_png_extension(filename))
        {
            // Add texture
            if (textures_add(textures, directory_path, filename) != 0)
            {
                fprintf(stderr, "Failed to add texture: %s\n", filename);
                // Continue loading other textures despite failure
            }
        }
    }

    closedir(dir);
    return 0;
}
