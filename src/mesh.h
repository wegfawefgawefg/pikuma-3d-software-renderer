#ifndef MESH_H
#define MESH_H

#include "sfa.h"
#include "mat4.h"
#include "su32a.h"

//////////////////////// PRIMITIVES ////////////////////////
// MESH: Combined Vertex and Index Data
typedef struct
{
    SFA *vertices;  // Vertex data
    SFA *normals;   // Normal data
    SFA *texcoords; // Texture coordinate data

    SU32A *vertex_indices;   // Index data
    SU32A *normal_indices;   // Normal index data
    SU32A *texcoord_indices; // Texture coordinate index data
    SU32A *colors;           // Color data
} Mesh;

// Function to create and free Mesh
Mesh *mesh_new(void);
Mesh *mesh_copy(const Mesh *mesh);
void mesh_free(Mesh *mesh);

//////////////////////// PROCEDURAL PRIMITIVES ////////////////////////
Mesh *mesh_prim_triangle(float size);
Mesh *mesh_prim_cube(float size);
Mesh *mesh_prim_tetrahedron(float size);
Mesh *mesh_prim_rectangle(float width, float height);

//////////////////////// MESH TRANSFORM FUNCTIONS ////////////////////////
void mesh_rotate(Mesh *mesh, Vec3 rotation);
void mesh_scale(Mesh *mesh, Vec3 scale);
void mesh_translate(Mesh *mesh, Vec3 translation);
void mesh_transform(Mesh *mesh, Mat4 transform);

Vec3 mesh_get_center(const Mesh *mesh);

#endif