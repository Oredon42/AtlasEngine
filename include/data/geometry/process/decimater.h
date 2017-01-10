#ifndef DECIMATER_H
#define DECIMATER_H

#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"
#include "OpenMesh/Core/Utils/Property.hh"

#include "topologicalmesh.h"

class Decimater
{
public:
    Decimater();

    void decimate(TopologicalMesh *topological_mesh, const float &percentage);
    float computeEOP(TopologicalMesh *topological_mesh, const OpenMesh::HalfedgeHandle &h_h);
};

#endif // DECIMATER_H
