#include "geometry/decimater.h"
#include "geometry/topologicalmesh.h"
#include "OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh"

#include <unordered_map>
#include <iostream>

using namespace OpenMesh;

/*
 * Error list element
 * contains Halfedge to collapse
 * error that will generate
 * and a boolean that says if it needs to be
 * recomputed
 * */
struct EOPElement
{
    HalfedgeHandle h_h;
    float eop;
    bool imprecise;

    EOPElement *next;

    EOPElement(HalfedgeHandle h, const float &e) :
        h_h(h),
        eop(e),
        imprecise(false),
        next(0)
    {

    }
};

/*
 * List of errors
 * must be sorted
 * */
struct EOPList
{
    EOPElement *begin;

    EOPList() :
        begin(0)
    {

    }

    ~EOPList()
    {
        if(begin != 0)
        {
            EOPElement *previous;
            do
            {
                previous = begin;
                begin = begin->next;
                delete previous;
            }
            while(begin != 0);
        }
    }

    void insert(EOPElement *element)
    {
        if(begin == 0)
        {
            begin = element;
            begin->next = 0;
        }
        else
        {
            element->next = begin;
            begin = element;

            /*EOPElement *current = begin;

            while(current->next != 0 && element->eop < current->eop)
                current = current->next;

            element->next = current->next;
            current->next = element;

            if(element->eop < current->eop)
            {
                EOPElement tmp = *current;
                *current = *element;
                *element = tmp;
            }*/
        }
    }

    EOPElement *pop()
    {
        EOPElement *out = begin;
        begin = begin->next;

        return out;
    }
};

Decimater::Decimater()
{

}

void Decimater::decimate(TopologicalMesh *topological_mesh, const float &percentage)
{
    const size_t goal_num_vertices = topological_mesh->n_vertices() * percentage;

    typedef std::unordered_map<HalfedgeHandle, EOPElement *> eopMap;

    eopMap eop_map;
    EOPList eop_list;

    /*
     * STEP 1
     * build eop list
     * */
    for(HalfedgeIter h_it = topological_mesh->halfedges_begin(); h_it != topological_mesh->halfedges_end(); ++h_it)
    {
        float eop = computeEOP(topological_mesh, *h_it);

        EOPElement *new_element = new EOPElement(*h_it, eop);

        eop_list.insert(new_element);
        eop_map[*h_it] = new_element;
    }

    /*
     * STEP 2
     * Decimate
     * */
    while(topological_mesh->n_vertices() > goal_num_vertices)
    {
        EOPElement *eop_element = eop_list.pop();

        if(!topological_mesh->is_valid_handle(eop_element->h_h))
        {
            delete eop_element;
        }
        else if(eop_element->imprecise)
        {
            eop_element->eop = computeEOP(topological_mesh, eop_element->h_h);
            eop_element->imprecise = false;
            eop_list.insert(eop_element);
        }
        else
        {
            //HalfedgeHandle  h_h = topological_mesh->opposite_halfedge_handle(eop_element->h_h);
            VertexHandle    v_h1 = topological_mesh->to_vertex_handle(eop_element->h_h);

            //  Set imprecise every halfedge affected by the collapse
            for(VertexOHalfedgeIter voh_it = topological_mesh->voh_iter(v_h1); voh_it; ++voh_it)
            {
                VertexHandle v_h2 = topological_mesh->to_vertex_handle(*voh_it);
                HalfedgeHandle h_h2 = topological_mesh->halfedge_handle(v_h2);

                eop_map[h_h2]->imprecise = true;

                for(VertexOHalfedgeIter voh_it2 = topological_mesh->voh_iter(v_h2); voh_it2; ++voh_it2)
                {
                    VertexHandle v_h3 = topological_mesh->to_vertex_handle(*voh_it2);
                    HalfedgeHandle h_h3 = topological_mesh->halfedge_handle(v_h3);

                    eop_map[h_h3]->imprecise = true;
                }
            }
            topological_mesh->collapse(eop_element->h_h);

            delete eop_element;
            topological_mesh->garbage_collection();
        }
    }
    topological_mesh->update_normals();
}

/*
 * Compute Error of Mesh between before and after
 * collapsing h_h
 * Error = sum of square dot products of adjacent vertices to h_h vertex
 * */
float Decimater::computeEOP(TopologicalMesh *topological_mesh, const HalfedgeHandle &h_h)
{
    float error = 0.f;

    HalfedgeHandle  h_h2 = topological_mesh->opposite_halfedge_handle(h_h);

    VertexHandle    v_h = topological_mesh->to_vertex_handle(h_h),
                    v_h2 = topological_mesh->to_vertex_handle(h_h2);

    Point v = topological_mesh->point(v_h);

    for(VertexOHalfedgeIter voh_it = topological_mesh->voh_iter(v_h2); voh_it; ++voh_it)
    {
        VertexHandle v_h3 = topological_mesh->to_vertex_handle(*voh_it);

        Point p = topological_mesh->point(v_h3);
        float dot_product = (p[0]*v[0] + p[1]*v[1] + p[2]*v[2]);
        error += dot_product * dot_product;
    }

    return error;
}
