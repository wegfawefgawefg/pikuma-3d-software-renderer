#define _DEFAULT_SOURCE // Ensure feature test macros are set

#include "material.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // For directory operations
#include <sys/types.h>
#include <errno.h>

MaterialLibrary material_library_load(const char *path)
{
    MaterialLibrary library;
    library.filename = NULL;
    library.materials = NULL;
    library.count = 0;

    FILE *file = fopen(path, "r");
    if (!file)
    {
        fprintf(stderr, "Failed to open .mtl file: %s\n", path);
        return library;
    }

    char line[MAX_LINE_LENGTH];
    Material current_material;
    int in_material = 0;
    size_t capacity = 0;

    // Extract filename from path
    const char *filename = strrchr(path, '/');
#ifdef _WIN32
    if (!filename)
    {
        filename = strrchr(path, '\\');
    }
#endif
    if (filename)
    {
        filename++; // Move past the '/' or '\\'
    }
    else
    {
        filename = path;
    }
    library.filename = duplicate_string(filename);

    while (fgets(line, sizeof(line), file))
    {
        // Trim the line
        char *trimmed = trim_whitespace(line);

        // Skip empty lines or comments
        if (trimmed[0] == '\0' || trimmed[0] == '#')
            continue;

        if (strncmp(trimmed, "newmtl", 6) == 0)
        {
            // If already in a material, save the previous one
            if (in_material)
            {
                // Ensure capacity
                if (library.count >= capacity)
                {
                    size_t new_capacity = capacity == 0 ? 10 : capacity * 2;
                    Material *temp = realloc(library.materials, new_capacity * sizeof(Material));
                    if (!temp)
                    {
                        fprintf(stderr, "Memory allocation failed while loading materials from %s.\n", path);
                        material_library_free(&library);
                        fclose(file);
                        return library;
                    }
                    library.materials = temp;
                    capacity = new_capacity;
                }
                // Add the material to the library
                library.materials[library.count++] = current_material;
            }

            // Start a new material
            memset(&current_material, 0, sizeof(Material));
            current_material.name = NULL;
            current_material.diffuse_map = NULL;
            current_material.ambient_color = vec3_create(0.0f, 0.0f, 0.0f);
            current_material.diffuse_color = vec3_create(0.0f, 0.0f, 0.0f);
            current_material.specular_color = vec3_create(0.0f, 0.0f, 0.0f);
            current_material.emissive_color = vec3_create(0.0f, 0.0f, 0.0f);
            in_material = 1;

            // Parse the material name
            char name[MAX_LINE_LENGTH];
            if (sscanf(trimmed, "newmtl %s", name) == 1)
            {
                current_material.name = duplicate_string(name);
            }
        }
        else if (in_material)
        {
            if (strncmp(trimmed, "Ns", 2) == 0)
            {
                sscanf(trimmed, "Ns %f", &current_material.shininess);
            }
            else if (strncmp(trimmed, "Ka", 2) == 0)
            {
                float r, g, b;
                if (sscanf(trimmed, "Ka %f %f %f", &r, &g, &b) == 3)
                {
                    current_material.ambient_color = vec3_create(r, g, b);
                }
            }
            else if (strncmp(trimmed, "Kd", 2) == 0)
            {
                float r, g, b;
                if (sscanf(trimmed, "Kd %f %f %f", &r, &g, &b) == 3)
                {
                    current_material.diffuse_color = vec3_create(r, g, b);
                }
            }
            else if (strncmp(trimmed, "Ks", 2) == 0)
            {
                float r, g, b;
                if (sscanf(trimmed, "Ks %f %f %f", &r, &g, &b) == 3)
                {
                    current_material.specular_color = vec3_create(r, g, b);
                }
            }
            else if (strncmp(trimmed, "Ke", 2) == 0)
            {
                float r, g, b;
                if (sscanf(trimmed, "Ke %f %f %f", &r, &g, &b) == 3)
                {
                    current_material.emissive_color = vec3_create(r, g, b);
                }
            }
            else if (strncmp(trimmed, "Ni", 2) == 0)
            {
                sscanf(trimmed, "Ni %f", &current_material.optical_density);
            }
            else if (strncmp(trimmed, "d ", 2) == 0 || strncmp(trimmed, "d\t", 2) == 0)
            {
                sscanf(trimmed, "d %f", &current_material.transparency);
            }
            else if (strncmp(trimmed, "illum", 5) == 0)
            {
                sscanf(trimmed, "illum %d", &current_material.illumination_model);
            }
            else if (strncmp(trimmed, "map_Kd", 6) == 0)
            {
                char map_path[MAX_LINE_LENGTH];
                if (sscanf(trimmed, "map_Kd %s", map_path) == 1)
                {
                    current_material.diffuse_map = duplicate_string(map_path);
                }
            }
        }
    }

    // After reading all lines, add the last material if any
    if (in_material)
    {
        // Ensure capacity
        if (library.count >= capacity)
        {
            size_t new_capacity = capacity == 0 ? 10 : capacity * 2;
            Material *temp = realloc(library.materials, new_capacity * sizeof(Material));
            if (!temp)
            {
                fprintf(stderr, "Memory allocation failed while loading materials from %s.\n", path);
                material_library_free(&library);
                fclose(file);
                return library;
            }
            library.materials = temp;
            capacity = new_capacity;
        }
        library.materials[library.count++] = current_material;
    }

    fclose(file);
    return library;
}

void material_library_free(MaterialLibrary *library)
{
    if (library == NULL)
        return;

    if (library->filename)
    {
        free(library->filename);
        library->filename = NULL;
    }

    if (library->materials)
    {
        for (size_t i = 0; i < library->count; i++)
        {
            free(library->materials[i].name);
            free(library->materials[i].diffuse_map);
            // If Vec3 contains dynamically allocated memory in the future, free it here
        }
        free(library->materials);
        library->materials = NULL;
    }
    library->count = 0;
}

void material_print(const Material *mat)
{
    if (mat == NULL)
    {
        printf("Null Material.\n");
        return;
    }

    printf("  Material: %s\n", mat->name ? mat->name : "Unnamed");
    printf("    Shininess (Ns): %.2f\n", mat->shininess);
    printf("    Ambient Color (Ka): %.2f %.2f %.2f\n",
           mat->ambient_color.x, mat->ambient_color.y, mat->ambient_color.z);
    printf("    Diffuse Color (Kd): %.2f %.2f %.2f\n",
           mat->diffuse_color.x, mat->diffuse_color.y, mat->diffuse_color.z);
    printf("    Specular Color (Ks): %.2f %.2f %.2f\n",
           mat->specular_color.x, mat->specular_color.y, mat->specular_color.z);
    printf("    Emissive Color (Ke): %.2f %.2f %.2f\n",
           mat->emissive_color.x, mat->emissive_color.y, mat->emissive_color.z);
    printf("    Optical Density (Ni): %.2f\n", mat->optical_density);
    printf("    Transparency (d): %.2f\n", mat->transparency);
    printf("    Illumination Model (illum): %d\n", mat->illumination_model);
    printf("    Diffuse Map (map_Kd): %s\n", mat->diffuse_map ? mat->diffuse_map : "None");
    printf("\n");
}

void material_library_print(const MaterialLibrary *library)
{
    if (library == NULL)
    {
        printf("Null MaterialLibrary.\n");
        return;
    }

    printf("Material Library: %s\n", library->filename ? library->filename : "Unnamed Library");
    printf("  %zu materials loaded.\n\n", library->count);
    for (size_t i = 0; i < library->count; i++)
    {
        printf("  Material %zu:\n", i + 1);
        material_print(&library->materials[i]);
    }
}

Material *material_library_get_material(const MaterialLibrary *library, const char *name)
{
    if (library == NULL || name == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < library->count; i++)
    {
        if (strcmp(library->materials[i].name, name) == 0)
        {
            return &library->materials[i];
        }
    }

    return NULL; // Not found
}

MaterialManager material_manager_init(size_t max_libraries)
{
    MaterialManager manager;
    manager.libraries = (MaterialLibrary *)malloc(max_libraries * sizeof(MaterialLibrary));
    if (manager.libraries == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for MaterialManager.\n");
        manager.count = 0;
        manager.max_libraries = 0;
        return manager;
    }
    manager.count = 0;
    manager.max_libraries = max_libraries;
    return manager;
}

int material_manager_load_from_directory(MaterialManager *manager, const char *directory_path)
{
    if (manager == NULL)
    {
        fprintf(stderr, "MaterialManager is NULL.\n");
        return -1;
    }

    DIR *dir = opendir(directory_path);
    if (dir == NULL)
    {
        fprintf(stderr, "Failed to open directory: %s\nError: %s\n", directory_path, strerror(errno));
        return -1;
    }

    struct dirent *entry;
    int success = 0;

    while ((entry = readdir(dir)) != NULL)
    {
        // Skip directories
        if (entry->d_type == DT_DIR)
            continue;

        // Check if the file has a .mtl extension
        const char *filename = entry->d_name;
        size_t len = strlen(filename);
        if (len < 4)
            continue; // Not enough length for ".mtl"

        if (strcasecmp(filename + len - 4, ".mtl") != 0)
            continue; // Not a .mtl file

        // Check if manager has space
        if (manager->count >= manager->max_libraries)
        {
            fprintf(stderr, "MaterialManager has reached its maximum capacity of %zu libraries.\n", manager->max_libraries);
            success = -1;
            break;
        }

        // Construct full path
        size_t path_len = strlen(directory_path);
        size_t file_len = strlen(filename);
        // +2 for possible '/' and null terminator
        char *full_path = (char *)malloc(path_len + 1 + file_len + 1);
        if (full_path == NULL)
        {
            fprintf(stderr, "Memory allocation failed for full path.\n");
            success = -1;
            break;
        }

        strcpy(full_path, directory_path);
        // Add '/' if not present
        if (directory_path[path_len - 1] != '/' && directory_path[path_len - 1] != '\\')
        {
            strcat(full_path, "/");
        }
        strcat(full_path, filename);

        // Load the material library
        MaterialLibrary library = material_library_load(full_path);
        if (library.materials == NULL)
        {
            fprintf(stderr, "Failed to load material library from file: %s\n", full_path);
            free(full_path);
            continue; // Skip to next file
        }

        // Assign to MaterialManager
        manager->libraries[manager->count++] = library;

        free(full_path);
    }

    closedir(dir);
    return success;
}

MaterialLibrary *material_manager_get_library(const MaterialManager *manager, const char *name)
{
    if (manager == NULL || name == NULL)
    {
        return NULL;
    }

    for (size_t i = 0; i < manager->count; i++)
    {
        if (strcmp(manager->libraries[i].filename, name) == 0)
        {
            return &manager->libraries[i];
        }
    }

    return NULL; // Not found
}

void material_manager_print(const MaterialManager *manager)
{
    if (manager == NULL)
    {
        printf("Null MaterialManager.\n");
        return;
    }

    printf("Material Manager: %zu material libraries loaded.\n\n", manager->count);
    for (size_t i = 0; i < manager->count; i++)
    {
        printf("Material Library %zu:\n", i + 1);
        material_library_print(&manager->libraries[i]);
    }
}

void material_manager_free(MaterialManager *manager)
{
    if (manager == NULL || manager->libraries == NULL)
        return;

    for (size_t i = 0; i < manager->count; i++)
    {
        material_library_free(&manager->libraries[i]);
    }
    free(manager->libraries);
    manager->libraries = NULL;
    manager->count = 0;
    manager->max_libraries = 0;
}
