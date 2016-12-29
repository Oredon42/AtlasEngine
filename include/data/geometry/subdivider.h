#ifndef SUBDIVIDER_H
#define SUBDIVIDER_H

#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"
#include "OpenMesh/Core/Utils/Property.hh"

#include "topologicalmesh.h"

class Subdivider
{
public:
    Subdivider();

    void subdivide(TopologicalMesh *topological_mesh, const unsigned int &num_iterations);
};

#endif // SUBDIVIDER_H
