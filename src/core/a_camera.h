#ifndef A_CAMERA_H
#define A_CAMERA_H

#include <glew.h>
#include <glm/glm.hpp>

class Shader;

class ACamera
{
public:
    //  Getters
    inline glm::vec3 getPosition() const{return m_vPosition;}
    inline glm::vec3 getFront() const{return m_vFront;}
    inline glm::vec3 getUp() const{return m_vUp;}
    inline glm::mat4 getView() const{return m_mView;}
    inline glm::mat3 getTransposedInvertedView() const{return m_mTransposedInvertedView;}
    inline glm::mat4 getProjection() const{return m_mProjection;}
    inline GLfloat getNear() const{return m_fNear;}
    inline GLfloat getFar() const{return m_fFar;}

    //  Setters
    void setPosition(const glm::vec3 &vPosition);
    void setOrientation(const glm::vec3 &vFront, const glm::vec3 &vUp);
    void setViewData(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp);
    void setNear(const GLfloat &fNear);
    void setFar(const GLfloat &fFar);

protected:
    //  Protected constructor
    ACamera(const glm::vec3 &vPosition = glm::vec3(0, 0, 0), const glm::vec3 &vFront = glm::vec3(1, 0, 0), const glm::vec3 &vUp = glm::vec3(0, 1, 0), const GLfloat &fNear = 0.1f, const GLfloat &fFar = 200.f);

    //  View
    glm::vec3 m_vPosition;
    glm::vec3 m_vFront;
    glm::vec3 m_vUp;

    glm::mat4 m_mView;
    glm::mat3 m_mTransposedInvertedView;

    //  Projection
    GLfloat m_fNear;
    GLfloat m_fFar;

    glm::mat4 m_mProjection;

    //  Protected methods
    void updateView();
    virtual void updateProjection() = 0;
};

#endif // A_CAMERA_H
