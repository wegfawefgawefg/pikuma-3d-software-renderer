#ifndef MODEL_H
#define MODEL_H

#include "mesh_data.h"
#include "shape.h"

// Model struct holds shape shared vertex data and an array of shapes
typedef struct
{
    MeshData *mesh_data; // vertices/normals/texcoords
    Shape *shapes;       // array of shapes (submeshes)
    int shape_count;
} Model;

Model *model_new(void);
void model_free(Model *model);

#endif