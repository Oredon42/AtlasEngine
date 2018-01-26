#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
    Transform();
    virtual ~Transform(){}

    virtual void setTransform(const glm::mat4 &mTransform);
    virtual void translate(const glm::vec3 &vTranslation);
    virtual void rotate(const glm::quat &qRotation);
    virtual void scale(const glm::vec3 &vScale);
    virtual void transform(const glm::mat4 &mTransform);

    //  Getters
    virtual inline glm::vec3 getPosition() const{return m_vPosition;}
    virtual inline glm::quat getOrientation() const{return m_qOrientation;}
    virtual inline glm::vec3 getScale() const{return m_vScale;}
    virtual inline glm::mat4 getTransform() const{return m_mTransform;}
    virtual inline glm::mat4 getInverseTransform() const{return m_mInverseTransform;}

protected:
    glm::mat4 m_mTransform;
    glm::mat4 m_mInverseTransform;
    glm::vec3 m_vPosition;
    glm::quat m_qOrientation;
    glm::vec3 m_vScale;

    //  Private methods
    void decompose();
};

#endif // TRANSFORM_H
