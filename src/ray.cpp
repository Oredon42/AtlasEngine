#include <stack>

#include "ray.h"
#include "kdtree.h"

Ray::Ray(glm::vec3 O) :
m_origine(O)
{

}

Ray::Ray(glm::vec3 O, glm::vec3 D) :
m_origine(O),
m_direction(D),
m_inv_direction(glm::vec3(1.f / D.x, 1.f / D.y, 1.f / D.z)),
m_sign_x(m_inv_direction.x < 0),
m_sign_y(m_inv_direction.y < 0),
m_sign_z(m_inv_direction.z < 0)
{

}

void Ray::setDirection(glm::vec3 D)
{
    m_direction = D;
    m_inv_direction = glm::vec3(1.f / D.x, 1.f / D.y, 1.f / D.z);
    m_sign_x = m_inv_direction.x < 0;
    m_sign_y = m_inv_direction.y < 0;
    m_sign_z = m_inv_direction.z < 0;
}

GLboolean Ray::intersectBox(const AABB &box) const
{
    GLfloat txmin, txmax, tymin, tymax, tzmin, tzmax;

    txmin = (box.V[m_sign_x].x - m_origine.x) * m_inv_direction.x;
    txmax = (box.V[1 - m_sign_x].x - m_origine.x) * m_inv_direction.x;
    tymin = (box.V[m_sign_y].y - m_origine.y) * m_inv_direction.y;
    tymax = (box.V[1 - m_sign_y].y - m_origine.y) * m_inv_direction.y;

    if((txmin > tymax) || (tymin > txmax))
        return false;
    if(tymin > txmin)
        txmin = tymin;
    if(tymax < txmax)
        txmax = tymax;

    tzmin = (box.V[m_sign_z].z - m_origine.z) * m_inv_direction.z;
    tzmax = (box.V[1 - m_sign_z].z - m_origine.z) * m_inv_direction.z;

    if((txmin > tzmax) || (tzmin > txmax))
        return false;

    return true;
}

GLboolean Ray::intersectBox(const AABB &box, GLfloat &tmin, GLfloat &tmax, const GLchar &dimension)
{
    GLfloat txmin, txmax, tymin, tymax, tzmin, tzmax;
    txmin = (box.V[m_sign_x].x - m_origine.x) * m_inv_direction.x;
    txmax = (box.V[1 - m_sign_x].x - m_origine.x) * m_inv_direction.x;
    tymin = (box.V[m_sign_y].y - m_origine.y) * m_inv_direction.y;
    tymax = (box.V[1 - m_sign_y].y - m_origine.y) * m_inv_direction.y;

    if((txmin > tymax) || (tymin > txmax))
        return false;
    if(tymin > txmin)
        txmin = tymin;
    if(tymax < txmax)
        txmax = tymax;

    tzmin = (box.V[m_sign_z].z - m_origine.z) * m_inv_direction.z;
    tzmax = (box.V[1 - m_sign_z].z - m_origine.z) * m_inv_direction.z;

    if((txmin > tzmax) || (tzmin > txmax))
        return false;
    if(tzmin > txmin)
        txmin = tzmin;
    if(tzmax < txmax)
        txmax = tzmax;

    if(dimension == DIMENSION_X)
    {
        tmin = txmin;
        tmax = txmax;
    }
    else if(dimension == DIMENSION_Y)
    {
        tmin = tymin;
        tmax = tymax;
    }
    else
    {
        tmin = tzmin;
        tmax = tzmax;
    }

    return true;
}

GLboolean Ray::intersectTriangle(const glm::vec3& V1, const glm::vec3& V2, const glm::vec3& V3, GLfloat& t, GLfloat& u, GLfloat& v)
{
    glm::vec3   O = m_origine,
                D = m_direction;
    glm::vec3 e1, e2;  //Edge1, Edge2
    glm::vec3 P, Q, T;
    GLfloat det, inv_det;

    //Find vectors for two edges sharing V1
    e1 = V2 - V1;
    e2 = V3 - V1;
    //Begin calculating determinant - also used to calculate u parameter
    P = glm::cross(D, e2);
    //if determinant is near zero, ray lies in plane of triangle
    det = glm::dot(e1, P);
    //NOT CULLING
    if(det > -EPSILON && det < EPSILON)
        return false;
    inv_det = 1.f / det;

    //calculate distance from V1 to ray origin
    T = O - V1;

    //Calculate u parameter and test bound
    u = glm::dot(T, P) * inv_det;
    //The intersection lies outside of the triangle
    if(u < 0.f || u > 1.f)
        return false;

    //Prepare to test v parameter
    Q = glm::cross(T, e1);

    //Calculate V parameter and test bound
    v = glm::dot(D, Q) * inv_det;
    //The intersection lies outside of the triangle
    if(v < 0.f || u + v  > 1.f)
        return false;

    t = glm::dot(e2, Q) * inv_det;

    if(t > EPSILON)
    {
        //ray intersection
        return true;
    }

    // No hit, no win
    return false;
}

GLboolean Ray::intersectSplit(const glm::vec3 &V1, const glm::vec3 &V2, const glm::vec3 &V3, GLfloat &t)
{
    glm::vec3   O = m_origine,
                D = m_direction;
    glm::vec3 e1, e2;  //Edge1, Edge2
    glm::vec3 P, Q, T;
    GLfloat u, v;
    GLfloat det, inv_det;

    //Find vectors for two edges sharing V1
    e1 = V2 - V1;
    e2 = V3 - V1;
    //Begin calculating determinant - also used to calculate u parameter
    P = glm::cross(D, e2);
    //if determinant is near zero, ray lies in plane of triangle
    det = glm::dot(e1, P);
    //NOT CULLING
    if(det > -EPSILON && det < EPSILON)
        return false;
    inv_det = 1.f / det;

    //calculate distance from V1 to ray origin
    T = O - V1;

    //Calculate u parameter and test bound
    u = glm::dot(T, P) * inv_det;

    //Prepare to test v parameter
    Q = glm::cross(T, e1);

    //Calculate V parameter and test bound
    v = glm::dot(D, Q) * inv_det;

    t = glm::dot(e2, Q) * inv_det;

    if(t > EPSILON)
    {
        //ray intersection
        return true;
    }

    // No hit, no win
    return false;
}

void Ray::intersectPlane(const Plane &p, GLfloat &tsplit)
{
    GLfloat distance_dimension;

    if(p.dimension == DIMENSION_X)
    {
        if(p.position == m_origine.x)
            tsplit = 0;
        else
        {
            //  Si signes différents
            if((p.position > 0 && m_origine.x < 0) || (p.position < 0 && m_origine.x > 0))
                distance_dimension = p.position - m_origine.x;
            //  Si négatif
            else if(p.position < 0 && m_origine.x < 0)
            {
                if(p.position < m_origine.x)
                    distance_dimension = p.position - m_origine.x;
                else
                    distance_dimension = -(m_origine.x - p.position);
            }
            else if(p.position > m_origine.x)
                distance_dimension = p.position - m_origine.x;
            else
                distance_dimension = m_origine.x - p.position;

            tsplit = fabs(distance_dimension * (1.f / m_direction.x));
        }
    }
    else if(p.dimension == DIMENSION_Y)
    {
        if(p.position == m_origine.y)
            tsplit = 0;
        else
        {
            //  Si signes différents
            if((p.position > 0 && m_origine.y < 0) || (p.position < 0 && m_origine.y > 0))
                distance_dimension = p.position - m_origine.y;
            //  Si négatif
            else if(p.position < 0 && m_origine.y < 0)
            {
                if(p.position < m_origine.y)
                    distance_dimension = p.position - m_origine.y;
                else
                    distance_dimension = -(m_origine.y - p.position);
            }
            else if(p.position > m_origine.y)
                distance_dimension = p.position - m_origine.y;
            else
                distance_dimension = m_origine.y - p.position;

            tsplit = fabs(distance_dimension * (1.f / m_direction.y));
        }
    }
    else
    {
        if(p.position == m_origine.z)
            tsplit = 0;
        else
        {
            //  Si signes différents
            if((p.position > 0 && m_origine.z < 0) || (p.position < 0 && m_origine.z > 0))
                distance_dimension = p.position - m_origine.z;
            //  Si négatif
            else if(p.position < 0 && m_origine.z < 0)
            {
                if(p.position < m_origine.z)
                    distance_dimension = p.position - m_origine.z;
                else
                    distance_dimension = -(m_origine.z - p.position);
            }
            else if(p.position > m_origine.z)
                distance_dimension = p.position - m_origine.z;
            else
                distance_dimension = m_origine.z - p.position;

            tsplit = fabs(distance_dimension * (1.f / m_direction.z));
        }
    }
}

GLboolean Ray::intersectKdTree(const KdTree &K, Intersection &intersection, Triangle &t)
{
    GLfloat tmin, tmax;
    std::stack<StackElement> stack;

    intersection.t = INFINITY;

    GLchar dir[3] = {coefDir(DIMENSION_X), coefDir(DIMENSION_Y), coefDir(DIMENSION_Z)};

    if(intersectBox(K.m_root->getBox(), tmin, tmax, K.m_root->getPlane().dimension))
    {
        StackElement root;
        root.node = K.m_root;
        root.t_min = tmin;
        root.t_max = tmax;
        stack.push(root);

        while(!stack.empty())
        {
            StackElement e = stack.top();
            stack.pop();
            KdTreeNode *n = e.node;
            tmin = e.t_min;
            tmax = e.t_max;

            //  On descend jusqu'à la feuille
            while(!n->isLeaf())
            {
                KdTreeNode *n_near, *n_far;
                GLuint dim = n->getPlane().dimension;

                if(dir[dim] == POSITIVE)
                {
                    n_near = n->getLeft();
                    n_far = n->getRight();
                }
                else
                {
                    n_near = n->getRight();
                    n_far = n->getLeft();
                }
                GLfloat tsplit;
                intersectPlane(n->getPlane(), tsplit);

                if(tsplit > tmax)
                    n = n_near;
                else if(tsplit < tmin)
                    n = n_far;
                else
                {
                    StackElement e2;
                    e2.node = n_far;
                    e2.t_min = tsplit;
                    e2.t_max = tmax;
                    stack.push(e2);
                    n = n_near;
                    tmax = tsplit;
                }
            }

            //  Intersection des triangles de la feuille
            size_t T_size = n->numTriangles();
            for(size_t i = 0; i < T_size; ++i)
            {
                Triangle t_tmp = *(n->getTriangle(i));
                Intersection intersection_tmp;
                if(intersectTriangle(t_tmp, intersection_tmp))
                {
                    //  Intersection la plus proche
                    if(intersection_tmp.t < intersection.t)
                    {
                        intersection = intersection_tmp;
                        intersection.V1 = *t_tmp.v1;
                        intersection.V2 = *t_tmp.v2;
                        intersection.V3 = *t_tmp.v3;
                        t = t_tmp;
                    }
                }
            }
            if(intersection.t < INFINITY)
            {
                while(!stack.empty())
                    stack.pop();

                return true;
            }

        }
        return false;
    }
    return false;
}

GLboolean Ray::doesIntersectKdTree(const KdTree &K)
{
    GLfloat tmin, tmax;
    std::stack<StackElement> stack;

    GLchar dir[3] = {coefDir(DIMENSION_X), coefDir(DIMENSION_Y), coefDir(DIMENSION_Z)};

    if(intersectBox(K.m_root->getBox(), tmin, tmax, K.m_root->getPlane().dimension))
    {
        StackElement root;
        root.node = K.m_root;
        root.t_min = tmin;
        root.t_max = tmax;
        stack.push(root);

        while(!stack.empty())
        {
            StackElement e = stack.top();
            stack.pop();
            KdTreeNode *n = e.node;
            tmin = e.t_min;
            tmax = e.t_max;

            //  On descend jusqu'à la feuille
            while(!n->isLeaf())
            {
                KdTreeNode *n_near, *n_far;
                GLuint dim = n->getPlane().dimension;

                if(dir[dim] == POSITIVE)
                {
                    n_near = n->getLeft();
                    n_far = n->getRight();
                }
                else
                {
                    n_near = n->getRight();
                    n_far = n->getLeft();
                }

                GLfloat tsplit;
                intersectPlane(n->getPlane(), tsplit);

                if(tsplit > tmax)
                    n = n_near;
                else if(tsplit < tmin)
                    n = n_far;
                else
                {
                    StackElement e2;
                    e2.node = n_far;
                    e2.t_min = tsplit;
                    e2.t_max = tmax;
                    stack.push(e2);
                    n = n_near;
                    tmax = tsplit;
                }
            }

            //  Intersection des triangles de la feuille
            size_t T_size = n->numTriangles();
            for(size_t i = 0; i < T_size; ++i)
            {
                Triangle t_tmp = *(n->getTriangle(i));
                Intersection intersection_tmp;
                if(intersectTriangle(t_tmp, intersection_tmp))
                {
                    //  Vidage de la pile
                    while(!stack.empty())
                        stack.pop();

                    return true;
                }
            }
        }
    }
    return false;
}

GLchar Ray::coefDir(const GLchar &dimension)
{
    if(dimension == DIMENSION_X)
    {
        if(m_direction.x > m_origine.x)
            return POSITIVE;
        else
            return NEGATIVE;
    }
    else if(dimension == DIMENSION_Y)
    {
        if(m_direction.y > m_origine.y)
            return POSITIVE;
        else
            return NEGATIVE;
    }
    else
    {
        if(m_direction.z > m_origine.z)
            return POSITIVE;
        else
            return NEGATIVE;
    }
}
