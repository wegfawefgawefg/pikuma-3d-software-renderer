#ifndef TEXTURE_MANAGEMENT_H
#define TEXTURE_MANAGEMENT_H

#include <stdint.h>
#include <SDL2/SDL.h>
#include "vec2.h"
#include "texture.h"

// Structure to hold individual texture properties
typedef struct
{
    char *path;       // Full path to the texture file
    char *filename;   // Filename of the texture
    Texture *texture; // Loaded Texture
} TextureManagerEntry;

// Structure to manage an array of textures
typedef struct
{
    TextureManagerEntry *entries; // Dynamic array of texture entries
    int num_entries;              // Current number of textures loaded
    int max_entries;              // Maximum capacity of the array
} TextureManager;

// Interface

//// Basic
/**
 * @brief Initializes a new TextureManager structure with a specified capacity.
 *
 * @param max_entries Maximum number of textures the array can hold.
 * @return TextureManager* Pointer to the initialized TextureManager structure, or NULL on failure.
 */
TextureManager *textures_new(int max_entries);

/**
 * @brief Frees all memory associated with a TextureManager structure.
 *
 * @param textures Pointer to the TextureManager structure to be freed.
 */
void textures_free(TextureManager *textures);

//// Loading
/**
 * @brief Loads all .png textures from a specified directory into the TextureManager array.
 *
 * @param textures Pointer to the TextureManager structure.
 * @param directory_path Path to the directory containing .png files.
 * @return int Returns 0 on success, -1 on failure.
 */
int textures_load_from_directory(TextureManager *textures, const char *directory_path);

//// Retrieval
/**
 * @brief Retrieves a Texture by its filename.
 *
 * @param textures Pointer to the TextureManager structure.
 * @param filename Name of the texture file to retrieve.
 * @return Texture* Pointer to the Texture if found, or NULL if not found.
 */
Texture *textures_get(TextureManager *textures, const char *filename);

//// Information
/**
 * @brief Prints details of all loaded textures.
 *
 * @param textures Pointer to the TextureManager structure.
 */
void textures_print(TextureManager *textures);

#endif // TEXTURE_MANAGEMENT_H
