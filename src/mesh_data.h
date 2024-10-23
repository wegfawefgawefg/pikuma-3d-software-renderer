#ifndef MESH_DATA_H
#define MESH_DATA_H

#include "sfa.h"

typedef struct
{
    SFA *vertices;
    SFA *normals;
    SFA *texcoords;
} MeshData;

MeshData *mesh_data_new(void);
void mesh_data_free(MeshData *mesh_data);

#endif // MESH_DATA_H