#ifndef MESH_H
#define MESH_H

#include "sfa.h"

//////////////////////// PRIMITIVES ////////////////////////
// MESH: Combined Vertex and Index Data
typedef struct
{
    SFA *vertices; // Vertex data
    SIA *indices;  // Index data
} Mesh;

// Function to create and free Mesh
Mesh *mesh_new(int vertex_length, int index_length);
void mesh_free(Mesh *mesh);

//////////////////////// MESH TRANSFORM FUNCTIONS ////////////////////////
void mesh_rotate(Mesh *mesh, Vec3 rotation);
void mesh_scale(Mesh *mesh, Vec3 scale);
void mesh_translate(Mesh *mesh, Vec3 translation);

//////////////////////// PROCEDURAL PRIMITIVES ////////////////////////
Mesh *mesh_prim_triangle(float size);
Mesh *mesh_prim_cube(float size);
Mesh *mesh_prim_tetrahedron(float size);

#endif // MESH_H