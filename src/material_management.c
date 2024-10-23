// #define _DEFAULT_SOURCE // Ensure feature test macros are set

#include "material_management.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // For directory operations
#include <sys/types.h>
#include <errno.h>

#include "material_library.h"
#include "utils.h"

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
