#include "include/data/geometryprocess.h"
#include "include/data/mesh.h"

#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
#include <OpenMesh/Core/Mesh/PolyMeshT.hh>
#include <OpenMesh/Core/Mesh/IteratorsT.hh>
#include <OpenMesh/Core/Mesh/TriMeshT.hh>

#include <unordered_map>

using namespace OpenMesh;

GeometryProcess::GeometryProcess()
{

}

Mesh *GeometryProcess::subdivide(const Mesh *mesh)
{
    TopologicalMesh *topological_mesh = TopologicalMeshFromMesh(mesh);

    topological_mesh->request_face_status();
    topological_mesh->request_edge_status();
    topological_mesh->request_vertex_status();


    for(TopologicalMesh::EdgeIter e_it=topological_mesh->edges_begin(); e_it!=topological_mesh->edges_end(); ++e_it)
    {
        if (!topological_mesh->is_boundary( *e_it ));
    }

    return 0;
}


TopologicalMesh *GeometryProcess::TopologicalMeshFromMesh(const Mesh *mesh)
{
    TopologicalMesh *topological_mesh = new TopologicalMesh();
    topological_mesh->request_halfedge_normals();

    struct comp_vec
    {
        bool operator()(const glm::vec3 &lhv, const glm::vec3 &rhv) const
        {
            return lhv.x<rhv.x || (lhv.x==rhv.x && lhv.y<rhv.y ) || (lhv.x==rhv.x && lhv.y==rhv.y && lhv.z<rhv.z );
        }
    };

    struct hash_vec
    {
        size_t operator()(const glm::vec3 &lvalue) const
        {
            return lvalue.x+lvalue.y+lvalue.z+
                    floor(lvalue.x)*1000.f+
                    floor(lvalue.y)*1000.f+
                    floor(lvalue.z)*1000.f;
        }
    };

    //typedef std::unordered_map<glm::vec3, TopologicalMesh::VertexHandle, comp_vec> vMap;
    typedef std::unordered_map<glm::vec3, TopologicalMesh::VertexHandle, hash_vec> vMap;
    vMap vertexHandles;

    std::vector<TopologicalMesh::VertexHandle> face_vhandles;

    assert(mesh->numIndices()%3 == 0);
    for(unsigned int i=0; i< mesh->numIndices(); ++i)
    {
        glm::vec3 p = mesh->getVertex(mesh->getIndex(i)).Position;
        vMap::iterator vtr = vertexHandles.find(p);
        TopologicalMesh::VertexHandle vh;
        if(vtr == vertexHandles.end())
        {
            vh = topological_mesh->add_vertex( TopologicalMesh::Point(p.x, p.y, p.z));
            vertexHandles.insert( vtr, vMap::value_type(p, vh) );
        }
        else
            vh = vtr->second;

        face_vhandles.push_back(vh);

        if(((i+1)%3)==0)
        {
            TopologicalMesh::FaceHandle fh = topological_mesh->add_face(face_vhandles);
            //!@warning fh halfedge points to the first vertex of vhandles, but it is not clearly specified
            TopologicalMesh::HalfedgeHandle hh = topological_mesh->halfedge_handle(fh);
            glm::vec3 n;

            n = mesh->getVertex(mesh->getIndex(i-2)).Normal;;
            topological_mesh->set_normal(hh, TopologicalMesh::Normal(n.x, n.y, n.z));

            hh = topological_mesh->next_halfedge_handle(hh);
            n = mesh->getVertex(mesh->getIndex(i-1)).Normal;
            topological_mesh->set_normal(hh, TopologicalMesh::Normal(n.x, n.y, n.z));

            hh = topological_mesh->next_halfedge_handle(hh);
            n = mesh->getVertex(mesh->getIndex(i)).Normal;
            topological_mesh->set_normal(hh, TopologicalMesh::Normal(n.x, n.y, n.z));

            face_vhandles.clear();
        }

    }
    assert(topological_mesh->n_faces() == mesh->numIndices() / 3);
}

Mesh *GeometryProcess::MeshFromTopologicalMesh(TopologicalMesh *topological_mesh)
{
    struct comp_vec
    {
        bool operator()(const Vertex &lhv, const Vertex &rhv) const
        {
            if( lhv.Position.x<rhv.Position.x || (lhv.Position.x==rhv.Position.x && lhv.Position.y<rhv.Position.y ) || (lhv.Position.x==rhv.Position.x && lhv.Position.y==rhv.Position.y && lhv.Position.z<rhv.Position.z ))
                return true;
            else if( lhv.Normal.x<rhv.Normal.x || (lhv.Normal.x==rhv.Normal.x && lhv.Normal.y<rhv.Normal.y ) || (lhv.Normal.x==rhv.Normal.x && lhv.Normal.y==rhv.Normal.y && lhv.Normal.z<rhv.Normal.z ))
                return true;
            else if( lhv.TexCoords.x<rhv.TexCoords.x || (lhv.TexCoords.x==rhv.TexCoords.x && lhv.TexCoords.y<rhv.TexCoords.y ))
                return true;
            return false;
        }
    };

    typedef std::map<Vertex, GLuint, comp_vec> vMap;
    vMap vertexHandles;

    //in->triangulate();
    topological_mesh->garbage_collection();
    topological_mesh->request_face_normals();
    topological_mesh->update_face_normals();
    topological_mesh->update_halfedge_normals(M_PI);
    topological_mesh->release_face_normals();

    std::vector<GLuint> meshIndices;
    std::vector<Vertex> meshVertices;

    // iterator over all faces
    unsigned int vertexIndex = 0;
    for(TopologicalMesh::FaceIter f_it=topological_mesh->faces_begin(); f_it!=topological_mesh->faces_end(); ++f_it)
    {
        Vertex v;
        GLuint indices[3];
        GLuint i=0;

        // iterator over vertex (thru haldedge to get access to halfedge normals)
        for(TopologicalMesh::FaceHalfedgeIter fv_it = topological_mesh->fh_iter(*f_it); fv_it.is_valid(); ++fv_it)
        {
            assert(i<3);
            TopologicalMesh::Point p = topological_mesh->point(topological_mesh->to_vertex_handle(*fv_it));
            TopologicalMesh::Normal n = topological_mesh->normal(*fv_it);
            v.Position = glm::vec3(p[0], p[1], p[2]);
            v.Normal = glm::vec3(n[0], n[1], n[2]);

            GLuint vi;
            vMap::iterator vtr = vertexHandles.find(v);
            if(vtr == vertexHandles.end())
            {
                vi = vertexIndex ++;
                vertexHandles.insert( vtr, vMap::value_type(v, vi));
                meshVertices.push_back(v);
            }
            else
                vi = vtr->second;

            indices[i] = vi;
            i++;
        }
        meshIndices.push_back(indices[0]);
        meshIndices.push_back(indices[1]);
        meshIndices.push_back(indices[2]);
    }
    assert(vertexIndex == meshVertices.size());

    return new Mesh(meshVertices, meshIndices, GL_FALSE);
}
