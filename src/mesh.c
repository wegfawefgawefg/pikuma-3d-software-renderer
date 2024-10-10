#include <stdlib.h>
#include <math.h>

#include "mesh.h"
#include "utils.h"
#include "mat4.h"

// Create a new Mesh
Mesh *mesh_new(int vertex_length, int index_length)
{
    Mesh *mesh = (Mesh *)malloc(sizeof(Mesh));
    if (!mesh)
        return NULL;

    mesh->vertices = sfa_new(vertex_length);
    if (!mesh->vertices)
    {
        free(mesh);
        return NULL;
    }

    mesh->indices = sia_new(index_length);
    if (!mesh->indices)
    {
        sfa_free(mesh->vertices);
        free(mesh);
        return NULL;
    }

    int num_faces = index_length / 3;
    // if index_length is zero set num_faces to 0
    if (index_length == 0)
    {
        num_faces = 0;
    }
    mesh->colors = su32a_new(num_faces);
    if (!mesh->colors)
    {
        sfa_free(mesh->vertices);
        sia_free(mesh->indices);
        free(mesh);
        return NULL;
    }

    return mesh;
}

// Free a Mesh
void mesh_free(Mesh *mesh)
{
    if (!mesh)
        return;

    sfa_free(mesh->vertices);
    sia_free(mesh->indices);
    su32a_free(mesh->colors);
    free(mesh);
}

//////////////////////// PRIMITIVE FUNCTIONS ////////////////////////
// just a 3d triangle centerd at 0,0,0
Mesh *mesh_prim_triangle(float size)
{
    // Number of vertices and indices for a tetrahedron
    const int num_vertices = 3;
    const int num_indices = 3; // 1 face * 3 indices per face

    // Allocate a new Mesh with space for vertices and indices
    Mesh *mesh = mesh_new(num_vertices * 3, num_indices);

    if (!mesh)
    {
        // Handle allocation failure
        return NULL;
    }

    // Calculate scaling factor to ensure edge length is 'size'
    // In a regular tetrahedron, edge length = 2 * sqrt(2) * a
    // where 'a' is the distance from the center to a vertex
    float a = size / (2.0f * sqrtf(2.0f));

    // Define vertices of a regular tetrahedron centered at the origin
    // Vertex 0: ( a,  a,  a)
    // Vertex 1: (-a, -a,  a)
    // Vertex 2: (-a,  a, -a)

    mesh->vertices->data[0] = a; // Vertex 0 X
    mesh->vertices->data[1] = a; // Vertex 0 Y
    mesh->vertices->data[2] = a; // Vertex 0 Z

    mesh->vertices->data[3] = -a; // Vertex 1 X
    mesh->vertices->data[4] = -a; // Vertex 1 Y
    mesh->vertices->data[5] = a;  // Vertex 1 Z

    mesh->vertices->data[6] = -a; // Vertex 2 X
    mesh->vertices->data[7] = a;  // Vertex 2 Y
    mesh->vertices->data[8] = -a; // Vertex 2 Z

    // Define indices for the three vertices
    // Each face is defined by three vertex indices
    mesh->indices->data[0] = 0;
    mesh->indices->data[1] = 1;
    mesh->indices->data[2] = 2;

    return mesh;
}

// Function to create a cube mesh with vertices and indices ordered clockwise when viewed from outside
Mesh *mesh_prim_cube(float size)
{
    // Define the number of vertices and indices
    // A cube has 8 unique vertices and 12 triangles (36 indices)
    int vertex_count = 8 * 3; // 8 vertices, 3 components each (x, y, z)
    int index_count = 36;     // 12 triangles * 3 indices

    // Create a new Mesh
    Mesh *cube = mesh_new(vertex_count, index_count);
    if (!cube)
        return NULL;

    float half = size / 2.0f;

    // Define the 8 vertices of the cube
    float vertices[] = {
        // Front face
        -half, -half, half, // Vertex 0
        half, -half, half,  // Vertex 1
        half, half, half,   // Vertex 2
        -half, half, half,  // Vertex 3
        // Back face
        -half, -half, -half, // Vertex 4
        half, -half, -half,  // Vertex 5
        half, half, -half,   // Vertex 6
        -half, half, -half   // Vertex 7
    };

    // Copy vertex data
    memcpy(cube->vertices->data, vertices, sizeof(vertices));

    // Define the 36 indices for 12 triangles (two per face)
    // Each face is defined with two triangles, ordered clockwise from outside
    unsigned int indices[] = {
        // Front face (z = half)
        0, 1, 2, // Triangle 1
        2, 3, 0, // Triangle 2

        // Back face (z = -half)
        5, 4, 7, // Triangle 3
        7, 6, 5, // Triangle 4

        // Left face (x = -half)
        4, 0, 3, // Triangle 5
        3, 7, 4, // Triangle 6

        // Right face (x = half)
        1, 5, 6, // Triangle 7
        6, 2, 1, // Triangle 8

        // Top face (y = half)
        3, 2, 6, // Triangle 9
        6, 7, 3, // Triangle 10

        // Bottom face (y = -half)
        4, 5, 1, // Triangle 11
        1, 0, 4  // Triangle 12
    };

    // Copy index data
    memcpy(cube->indices->data, indices, sizeof(indices));

    return cube;
}

Mesh *mesh_prim_tetrahedron(float size)
{
    // Number of vertices and indices for a tetrahedron
    const int num_vertices = 4;
    const int num_indices = 12; // 4 faces * 3 indices per face

    // Allocate a new Mesh with space for vertices and indices
    Mesh *mesh = mesh_new(num_vertices * 3, num_indices);

    if (!mesh)
    {
        // Handle allocation failure
        return NULL;
    }

    // Calculate scaling factor to ensure edge length is 'size'
    // In a regular tetrahedron, edge length = 2 * sqrt(2) * a
    // where 'a' is the distance from the center to a vertex
    float a = size / (2.0f * sqrtf(2.0f));

    // Define vertices of a regular tetrahedron centered at the origin
    // Vertex 0: ( a,  a,  a)
    // Vertex 1: (-a, -a,  a)
    // Vertex 2: (-a,  a, -a)
    // Vertex 3: ( a, -a, -a)

    mesh->vertices->data[0] = a; // Vertex 0 X
    mesh->vertices->data[1] = a; // Vertex 0 Y
    mesh->vertices->data[2] = a; // Vertex 0 Z

    mesh->vertices->data[3] = -a; // Vertex 1 X
    mesh->vertices->data[4] = -a; // Vertex 1 Y
    mesh->vertices->data[5] = a;  // Vertex 1 Z

    mesh->vertices->data[6] = -a; // Vertex 2 X
    mesh->vertices->data[7] = a;  // Vertex 2 Y
    mesh->vertices->data[8] = -a; // Vertex 2 Z

    mesh->vertices->data[9] = a;   // Vertex 3 X
    mesh->vertices->data[10] = -a; // Vertex 3 Y
    mesh->vertices->data[11] = -a; // Vertex 3 Z

    // Define indices for the four triangular faces
    // Each face is defined by three vertex indices

    // Face 0: Vertex 0, Vertex 1, Vertex 2
    mesh->indices->data[0] = 0;
    mesh->indices->data[1] = 1;
    mesh->indices->data[2] = 2;

    // Face 1: Vertex 0, Vertex 2, Vertex 3
    mesh->indices->data[3] = 0;
    mesh->indices->data[4] = 2;
    mesh->indices->data[5] = 3;

    // Face 2: Vertex 0, Vertex 3, Vertex 1
    mesh->indices->data[6] = 0;
    mesh->indices->data[7] = 3;
    mesh->indices->data[8] = 1;

    // Face 3: Vertex 1, Vertex 3, Vertex 2
    mesh->indices->data[9] = 1;
    mesh->indices->data[10] = 3;
    mesh->indices->data[11] = 2;

    return mesh;
}

// Function to create a rectangular mesh centered at the origin in the XY plane
Mesh *mesh_prim_rectangle(float width, float height)
{
    // Number of vertices and indices for a rectangle
    const int num_vertices = 4;
    const int num_indices = 6; // 2 triangles * 3 indices each

    // Allocate a new Mesh with space for vertices and indices
    Mesh *mesh = mesh_new(num_vertices * 3, num_indices);
    if (!mesh)
    {
        // Handle allocation failure
        return NULL;
    }

    // Calculate half dimensions
    float half_width = width / 2.0f;
    float half_height = height / 2.0f;

    // Define the 4 vertices of the rectangle in the XY plane (Z = 0)
    float vertices[] = {
        // Vertex 0: Bottom-left
        -half_width, -half_height, 0.0f,
        // Vertex 1: Bottom-right
        half_width, -half_height, 0.0f,
        // Vertex 2: Top-right
        half_width, half_height, 0.0f,
        // Vertex 3: Top-left
        -half_width, half_height, 0.0f};

    // Copy vertex data into the mesh
    memcpy(mesh->vertices->data, vertices, sizeof(vertices));

    // Define the 6 indices for the two triangles
    unsigned int indices[] = {
        // Triangle 1
        0, 1, 2,
        // Triangle 2
        2, 3, 0};

    // Copy index data into the mesh
    memcpy(mesh->indices->data, indices, sizeof(indices));

    return mesh;
}

//////////////////////// MESH TRANSFORM FUNCTIONS ////////////////////////
// Function to rotate the mesh by the given Euler angles (in degrees)
void mesh_rotate(Mesh *mesh, Vec3 rotation)
{
    if (!mesh || !mesh->vertices || !mesh->vertices->data)
    {
        return; // Handle invalid mesh
    }

    // Convert rotation angles from degrees to radians
    float rx = degrees_to_radians(rotation.x);
    float ry = degrees_to_radians(rotation.y);
    float rz = degrees_to_radians(rotation.z);

    // Precompute sine and cosine of rotation angles
    float cosx = cosf(rx);
    float sinx = sinf(rx);
    float cosy = cosf(ry);
    float siny = sinf(ry);
    float cosz = cosf(rz);
    float sinz = sinf(rz);

    // Iterate over each vertex and apply rotation
    for (int i = 0; i < mesh->vertices->length; i += 3)
    {
        // Original coordinates
        float x = mesh->vertices->data[i];
        float y = mesh->vertices->data[i + 1];
        float z = mesh->vertices->data[i + 2];

        // Rotation around X-axis
        float y1 = y * cosx - z * sinx;
        float z1 = y * sinx + z * cosx;

        // Rotation around Y-axis
        float x2 = x * cosy + z1 * siny;
        float z2 = -x * siny + z1 * cosy;

        // Rotation around Z-axis
        float x3 = x2 * cosz - y1 * sinz;
        float y3 = x2 * sinz + y1 * cosz;

        // Update vertex coordinates
        mesh->vertices->data[i] = x3;
        mesh->vertices->data[i + 1] = y3;
        mesh->vertices->data[i + 2] = z2;
    }
}

// Function to scale the mesh by the given scale factors
void mesh_scale(Mesh *mesh, Vec3 scale)
{
    if (!mesh || !mesh->vertices || !mesh->vertices->data)
    {
        return; // Handle invalid mesh
    }

    // Iterate over each vertex and apply scaling
    for (int i = 0; i < mesh->vertices->length; i += 3)
    {
        mesh->vertices->data[i] *= scale.x;     // Scale X
        mesh->vertices->data[i + 1] *= scale.y; // Scale Y
        mesh->vertices->data[i + 2] *= scale.z; // Scale Z
    }
}

// Function to translate the mesh by the given translation vector
void mesh_translate(Mesh *mesh, Vec3 translation)
{
    if (!mesh || !mesh->vertices || !mesh->vertices->data)
    {
        return; // Handle invalid mesh
    }

    // Iterate over each vertex and apply translation
    for (int i = 0; i < mesh->vertices->length; i += 3)
    {
        mesh->vertices->data[i] += translation.x;     // Translate X
        mesh->vertices->data[i + 1] += translation.y; // Translate Y
        mesh->vertices->data[i + 2] += translation.z; // Translate Z
    }
}

Vec3 mesh_get_center(const Mesh *mesh)
{
    Vec3 center = {0.0f, 0.0f, 0.0f};

    if (!mesh || !mesh->vertices || !mesh->vertices->data)
    {
        return center; // Handle invalid mesh
    }

    // Calculate the center of the mesh by averaging all vertices
    for (int i = 0; i < mesh->vertices->length; i += 3)
    {
        center.x += mesh->vertices->data[i];
        center.y += mesh->vertices->data[i + 1];
        center.z += mesh->vertices->data[i + 2];
    }

    center.x /= mesh->vertices->length / 3;
    center.y /= mesh->vertices->length / 3;
    center.z /= mesh->vertices->length / 3;

    return center;
}

// Function to create a deep copy of the given mesh
Mesh *mesh_copy(const Mesh *mesh)
{
    if (!mesh)
    {
        return NULL; // Handle invalid mesh
    }

    // Create a new mesh with the same number of vertices and indices
    Mesh *copy = mesh_new(mesh->vertices->length, mesh->indices->length);
    if (!copy)
    {
        return NULL; // Handle allocation failure
    }

    // Copy vertex and index data
    memcpy(copy->vertices->data, mesh->vertices->data, mesh->vertices->length * sizeof(float));
    memcpy(copy->indices->data, mesh->indices->data, mesh->indices->length * sizeof(int));
    memcpy(copy->colors->data, mesh->colors->data, mesh->colors->length * sizeof(uint32_t));

    return copy;
}

// mesh_transform, takes in a mesh and a transformation mat4 and applies it
void mesh_transform(Mesh *mesh, Mat4 transform)
{
    if (!mesh || !mesh->vertices || !mesh->vertices->data)
    {
        return; // Handle invalid mesh
    }

    // Iterate over each vertex and apply transformation
    for (int i = 0; i < mesh->vertices->length; i += 3)
    {
        Vec3 vertex = {mesh->vertices->data[i], mesh->vertices->data[i + 1], mesh->vertices->data[i + 2]};
        Vec3 transformed = mat4_multiply_vec3(transform, vertex);

        mesh->vertices->data[i] = transformed.x;
        mesh->vertices->data[i + 1] = transformed.y;
        mesh->vertices->data[i + 2] = transformed.z;
    }
}
