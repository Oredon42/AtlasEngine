#ifndef RAY_H
#define RAY_H

#include "openglincludes.h"

#define GLM_FORCE_RADIANS
#include <lib/glm/glm.hpp>
#include <lib/glm/gtc/matrix_transform.hpp>
#include <lib/glm/gtc/type_ptr.hpp>

#include "kdtree.h"
#include "kdtreenode.h"

#define RAY_MAX_DEPTH 3

#define ACNEEPSILON 0.001f
#define GLOBALFACTOR 0.75f

#define NEGATIVE 0
#define POSITIVE 1

struct StackElement
{
    KdTreeNode *node;
    GLfloat t_min;
    GLfloat t_max;
};

struct Intersection
{
    GLfloat u;
    GLfloat v;
    GLfloat t;
    Vertex V1;
    Vertex V2;
    Vertex V3;
};

class Ray
{
public:
    Ray(glm::vec3 O);
    Ray(glm::vec3 O, glm::vec3 D);

    inline void setOrigin(const glm::vec3 &O){m_origine = O;}
    void setDirection(glm::vec3 D);

    inline glm::vec3 getOrigin() const{return m_origine;}
    inline glm::vec3 getDirection() const{return m_direction;}

    GLboolean intersectBox(const AABB &box) const;
    GLboolean intersectBox(const AABB &box, GLfloat &tmin, GLfloat &tmax, const GLchar &dimension);
    GLboolean intersectTriangle(const glm::vec3 &V1, const glm::vec3 &V2, const glm::vec3 &V3, GLfloat &t, GLfloat &u, GLfloat &v);
    inline GLboolean intersectTriangle(const Triangle &T, Intersection &intersection){return intersectTriangle(T.v1->Position, T.v2->Position, T.v3->Position, intersection.t, intersection.u, intersection.v);}
    void intersectPlane(const Plane &p, GLfloat &tsplit);
    GLboolean intersectKdTree(const KdTree &K, Intersection &intersection, Triangle &t);
    GLboolean doesIntersectKdTree(const KdTree &K);
    GLchar coefDir(const GLchar &dimension);

    GLboolean intersectSplit(const glm::vec3 &V1, const glm::vec3 &V2, const glm::vec3 &V3, GLfloat &t);

private:
    glm::vec3 m_origine;
    glm::vec3 m_direction;

    glm::vec3 m_inv_direction;

    int m_sign_x;
    int m_sign_y;
    int m_sign_z;
};

#endif // RAY_H
