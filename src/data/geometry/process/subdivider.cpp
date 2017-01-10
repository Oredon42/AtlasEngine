#include "include/data/geometry/process/subdivider.h"

#include <OpenMesh/Core/Geometry/VectorT.hh>
#include <OpenMesh/Core/Mesh/PolyConnectivity.hh>
#include <OpenMesh/Core/Mesh/PolyMeshT.hh>
#include <OpenMesh/Core/Mesh/IteratorsT.hh>
#include <OpenMesh/Core/Mesh/TriMeshT.hh>
#include <OpenMesh/Core/Mesh/Traits.hh>

#include <unordered_map>

#include <iostream>

using namespace OpenMesh;

Subdivider::Subdivider()
{

}

void Subdivider::subdivide(TopologicalMesh *topological_mesh, const unsigned int &num_iterations)
{
    typedef std::unordered_map<EdgeHandle, VertexHandle> oddMap;


    for(unsigned int i = 0; i < num_iterations; ++i)
    {
        VertexIter v_end = topological_mesh->vertices_end();
        oddMap odd_vertices_map;

        /*
         * STEP 1
         * compute odd vertices
         * */
        for(EdgeIter e_it = topological_mesh->edges_begin(); e_it != topological_mesh->edges_end(); ++e_it)
        {
            HalfedgeHandle      h_h0 = topological_mesh->halfedge_handle(*e_it, 0),
                                h_h1 = topological_mesh->halfedge_handle(*e_it, 1);

            VertexHandle        v_h0 = topological_mesh->to_vertex_handle(h_h0),
                                v_h1 = topological_mesh->to_vertex_handle(h_h1);

            Point               p0 = topological_mesh->point(v_h0),
                                p1 = topological_mesh->point(v_h1),
                                p_odd;

            if(topological_mesh->is_boundary(*e_it))
                p_odd = 0.5f*p0 + 0.5f*p1;
            else
            {
                HalfedgeHandle  h_h2 = topological_mesh->next_halfedge_handle(h_h0),
                                h_h3 = topological_mesh->next_halfedge_handle(h_h1);

                VertexHandle    v_h2 = topological_mesh->to_vertex_handle(h_h2),
                                v_h3 = topological_mesh->to_vertex_handle(h_h3);

                Point           p2 = topological_mesh->point(v_h2),
                                p3 = topological_mesh->point(v_h3);

                p_odd = 0.375f*p0 + 0.375f*p1 + 0.125f*p2 + 0.125f*p3;
            }

            //  Add odd vertex to map
            VertexHandle vh = topological_mesh->add_vertex(p_odd);
            odd_vertices_map[*e_it] = vh;
        }

        /*
         * STEP 2
         * compute new even vertices coords
         * */
        for(VertexIter v_it = topological_mesh->vertices_begin(); v_it != v_end; ++v_it)
        {
            Point p_even = Point(0);

            for(VertexOHalfedgeIter voh_it = topological_mesh->voh_iter(*v_it); voh_it; ++voh_it)
            {
                VertexHandle v_h = topological_mesh->to_vertex_handle(*voh_it);
                p_even += topological_mesh->point(v_h);
            }

            if(topological_mesh->is_boundary(*v_it))
                p_even = p_even*0.125f + topological_mesh->point(*v_it)*0.75f;
            else
            {
                int n = topological_mesh->valence(*v_it);
                float beta = (n>3)? 3.f/(8.f*n) : 0.1875f;

                p_even = p_even*beta + topological_mesh->point(*v_it)*(1.f-n*beta);
            }

            //  Set even vertex
            topological_mesh->set_point(*v_it, p_even);
        }

        /*
         * STEP 3
         * Create new faces
         * */
        FaceIter faces_end = topological_mesh->faces_end();
        for(FaceIter f_it = topological_mesh->faces_begin(); f_it != faces_end; ++f_it)
        {
            HalfedgeHandle  h_h0 = topological_mesh->halfedge_handle(*f_it),
                            h_h1 = topological_mesh->next_halfedge_handle(h_h0),
                            h_h2 = topological_mesh->next_halfedge_handle(h_h1);

            EdgeHandle      e_h0 = topological_mesh->edge_handle(h_h0),
                            e_h1 = topological_mesh->edge_handle(h_h1),
                            e_h2 = topological_mesh->edge_handle(h_h2);

            VertexHandle    v_h0 = topological_mesh->to_vertex_handle(h_h0),
                            v_h1 = topological_mesh->to_vertex_handle(h_h1),
                            v_h2 = topological_mesh->to_vertex_handle(h_h2);

            VertexHandle    v_h3 = odd_vertices_map.at(e_h0),
                            v_h4 = odd_vertices_map.at(e_h1),
                            v_h5 = odd_vertices_map.at(e_h2);

            topological_mesh->delete_face(*f_it);

            std::vector<VertexHandle>   face_0,
                                        face_1,
                                        face_2,
                                        face_3;

            face_0.push_back(v_h0); face_1.push_back(v_h1); face_2.push_back(v_h2); face_3.push_back(v_h3);
            face_0.push_back(v_h4); face_1.push_back(v_h5); face_2.push_back(v_h3); face_3.push_back(v_h4);
            face_0.push_back(v_h3); face_1.push_back(v_h4); face_2.push_back(v_h5); face_3.push_back(v_h5);

            topological_mesh->add_face(face_0),
            topological_mesh->add_face(face_1),
            topological_mesh->add_face(face_2),
            topological_mesh->add_face(face_3);

            face_0.clear();
            face_1.clear();
            face_2.clear();
            face_3.clear();
        }
        topological_mesh->garbage_collection();
    }

    topological_mesh->update_normals();
}
