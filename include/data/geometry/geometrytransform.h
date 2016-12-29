#ifndef GEOMETRYTRANSFORM_H
#define GEOMETRYTRANSFORM_H

#include "topologicalmesh.h"
#include "subdivider.h"

class Mesh;

/*
 * GeometryTransform class
 * */
class GeometryTransform
{
public:
    GeometryTransform();

    Mesh *getSubdividedMesh(const Mesh *mesh, const unsigned int &num_iterations);
    void subdivideMesh(Mesh *mesh, const unsigned int &num_iterations);

private:
    TopologicalMesh *TopologicalMeshFromMesh(const Mesh *mesh);
    Mesh *MeshFromTopologicalMesh(TopologicalMesh *topological_mesh);

    Subdivider m_subdivider;
};

#endif // GEOMETRYTRANSFORM_H
