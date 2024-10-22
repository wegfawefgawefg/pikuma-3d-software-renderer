#ifndef MATERIAL_H
#define MATERIAL_H

#include <stddef.h>
#include "vec3.h"

// Maximum line length for reading the .mtl file
#define MAX_LINE_LENGTH 256

// Structure to hold individual material properties
typedef struct
{
    char *name;             // Material name
    float shininess;        // Shininess (Ns)
    Vec3 ambient_color;     // Ambient color (Ka)
    Vec3 diffuse_color;     // Diffuse color (Kd)
    Vec3 specular_color;    // Specular color (Ks)
    Vec3 emissive_color;    // Emissive color (Ke)
    float optical_density;  // Optical density (Ni)
    float transparency;     // Transparency (d)
    int illumination_model; // Illumination model (illum)
    char *diffuse_map;      // Diffuse texture map (map_Kd)
} Material;

// Structure to represent a material library loaded from a .mtl file
typedef struct
{
    char *filename;      // Name of the .mtl file
    Material *materials; // Dynamic array of materials
    size_t count;        // Number of materials loaded
} MaterialLibrary;

// Structure to manage multiple material libraries
typedef struct
{
    MaterialLibrary *libraries; // Array of material libraries
    size_t count;               // Number of libraries loaded
    size_t max_libraries;       // Maximum number of libraries the manager can hold
} MaterialManager;

/**
 * @brief Loads materials from a .mtl file into a MaterialLibrary.
 *
 * @param path Path to the .mtl file.
 * @return MaterialLibrary containing the loaded materials and filename.
 *         If loading fails, filename is NULL, materials is NULL, and count is 0.
 */
MaterialLibrary material_library_load(const char *path);

/**
 * @brief Frees the memory allocated for a MaterialLibrary.
 *
 * @param library Pointer to the MaterialLibrary to be freed.
 */
void material_library_free(MaterialLibrary *library);

/**
 * @brief Prints the details of a single Material.
 *
 * @param mat Pointer to the Material to be printed.
 */
void material_print(const Material *mat);

/**
 * @brief Prints the details of a MaterialLibrary.
 *
 * @param library Pointer to the MaterialLibrary to be printed.
 */
void material_library_print(const MaterialLibrary *library);

/**
 * @brief Retrieves a Material by the material's name from a MaterialLibrary.
 *
 * @param library Pointer to the MaterialLibrary.
 * @param name Name of the material to retrieve.
 */
Material *material_library_get_material(const MaterialLibrary *library, const char *name);

/**
 * @brief Initializes a MaterialManager with a specified maximum number of libraries.
 *
 * @param max_libraries The maximum number of material libraries the manager can hold.
 * @return MaterialManager with allocated memory for libraries.
 *         If allocation fails, libraries is NULL, count is 0, and max_libraries is 0.
 */
MaterialManager material_manager_init(size_t max_libraries);

/**
 * @brief Loads all .mtl files from a specified directory into the MaterialManager.
 *
 * @param manager Pointer to the MaterialManager.
 * @param directory_path Path to the directory containing .mtl files.
 * @return int Returns 0 on success, -1 on failure.
 */
int material_manager_load_from_directory(MaterialManager *manager, const char *directory_path);

/**
 * @brief Retrieves a MaterialLibrary by the library's filename.
 *
 * @param manager Pointer to the MaterialManager.
 * @param name Name of the .mtl file to retrieve (e.g., "Peaches_Castle.mtl").
 * @return MaterialLibrary* Pointer to the found MaterialLibrary, or NULL if not found.
 */
MaterialLibrary *material_manager_get_library(const MaterialManager *manager, const char *name);

/**
 * @brief Frees the memory allocated for a MaterialManager and all its libraries.
 *
 * @param manager Pointer to the MaterialManager to be freed.
 */
void material_manager_free(MaterialManager *manager);

/**
 * @brief Prints the details of all MaterialLibraries managed by the MaterialManager.
 *
 * @param manager Pointer to the MaterialManager to be printed.
 */
void material_manager_print(const MaterialManager *manager);

#endif // MATERIAL_H
