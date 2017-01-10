#ifndef GEOMETRYTRANSFORM_H
#define GEOMETRYTRANSFORM_H

#include "topologicalmesh.h"
#include "subdivider.h"
#include "decimater.h"

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

    Mesh *getDecimatedMesh(const Mesh *mesh, const float &percentage);
    void decimateMesh(Mesh *mesh, const float &percentage);

private:
    TopologicalMesh *TopologicalMeshFromMesh(const Mesh *mesh);
    Mesh *MeshFromTopologicalMesh(TopologicalMesh *topological_mesh);

    Subdivider m_subdivider;
    Decimater m_decimater;
};

#endif // GEOMETRYTRANSFORM_H
