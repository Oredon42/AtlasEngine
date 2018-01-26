#ifndef DECIMATER_H
#define DECIMATER_H

struct TopologicalMeshTraits;
namespace OpenMesh
{
    template <class Traits>
    class TriMesh_ArrayKernelT;
}
typedef OpenMesh::TriMesh_ArrayKernelT<TopologicalMeshTraits> TopologicalMesh;

namespace OpenMesh
{
    struct HalfedgeHandle;
}

class Decimater
{
public:
    Decimater();

    void decimate(TopologicalMesh *topological_mesh, const float &percentage);
    float computeEOP(TopologicalMesh *topological_mesh, const OpenMesh::HalfedgeHandle &h_h);
};

#endif // DECIMATER_H
