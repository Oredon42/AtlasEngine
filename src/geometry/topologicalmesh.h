#ifndef TOPOLOGICALMESH_H
#define TOPOLOGICALMESH_H

#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"

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
typedef TopologicalMesh::VertexIter VertexIter;
typedef TopologicalMesh::VertexOHalfedgeIter VertexOHalfedgeIter;
typedef TopologicalMesh::HalfedgeIter HalfedgeIter;
typedef TopologicalMesh::EdgeIter EdgeIter;
typedef TopologicalMesh::FaceIter FaceIter;
typedef TopologicalMesh::FaceHalfedgeIter FaceHalfedgeIter;
typedef TopologicalMesh::Point Point;
typedef TopologicalMesh::Normal Normal;


#endif // TOPOLOGICALMESH_H
