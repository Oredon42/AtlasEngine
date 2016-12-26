#ifndef GEOMETRYTRANSFORM_H
#define GEOMETRYTRANSFORM_H

#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"
#include "OpenMesh/Core/Utils/Property.hh"

class Mesh;

struct TopologicalMeshTraits : public OpenMesh::DefaultTraits
{
  typedef OpenMesh::Vec3f Point;
  typedef OpenMesh::Vec3f Normal;
  typedef OpenMesh::Vec2f TexCoord;

  VertexAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
  FaceAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
  EdgeAttributes(OpenMesh::Attributes::Status);
  HalfedgeAttributes(OpenMesh::Attributes::Status | OpenMesh::Attributes::Normal);
};
typedef OpenMesh::TriMesh_ArrayKernelT<TopologicalMeshTraits> TopologicalMesh;

/*
 * GeometryTransform class
 * */
class GeometryTransform
{
public:
    GeometryTransform();

    Mesh *subdivide(const Mesh *mesh);

private:
    TopologicalMesh *TopologicalMeshFromMesh(const Mesh *mesh);
    Mesh *MeshFromTopologicalMesh(TopologicalMesh *topological_mesh);
};

#endif // GEOMETRYTRANSFORM_H
