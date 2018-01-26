#ifndef SUBDIVIDER_H
#define SUBDIVIDER_H

struct TopologicalMeshTraits;
namespace OpenMesh
{
    template <class Traits>
    class TriMesh_ArrayKernelT;
}
typedef OpenMesh::TriMesh_ArrayKernelT<TopologicalMeshTraits> TopologicalMesh;

class Subdivider
{
public:
    Subdivider();

    void subdivide(TopologicalMesh *topological_mesh, const unsigned int &num_iterations);
};

#endif // SUBDIVIDER_H
