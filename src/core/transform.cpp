#include "transform.h"

#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtx/matrix_decompose.hpp"

Transform::Transform() :
    m_mTransform(1.f),
    m_mInverseTransform(glm::inverse(m_mTransform))
{
    decompose();
}

void Transform::setTransform(const glm::mat4 &mTransform)
{
    m_mTransform = mTransform;
    m_mInverseTransform = glm::inverse(m_mTransform);
    decompose();
}

void Transform::translate(const glm::vec3 &vTranslation)
{
    m_mTransform = glm::translate(m_mTransform, vTranslation);
    m_mInverseTransform = glm::inverse(m_mTransform);
    decompose();
}

void Transform::rotate(const glm::quat &qRotation)
{
    m_mTransform *= glm::mat4_cast(qRotation);
    m_mInverseTransform = glm::inverse(m_mTransform);
    decompose();
}

void Transform::scale(const glm::vec3 &vScale)
{
    m_mTransform = glm::scale(m_mTransform, vScale);
    m_mInverseTransform = glm::inverse(m_mTransform);
    decompose();
}

void Transform::transform(const glm::mat4 &mTransform)
{
    m_mTransform *= mTransform;
    m_mInverseTransform = glm::inverse(m_mTransform);
    decompose();
}

void Transform::decompose()
{
    glm::vec3 vSkew;
    glm::vec4 vPerspective;
    glm::decompose(m_mTransform, m_vScale, m_qOrientation, m_vPosition, vSkew, vPerspective);
    m_qOrientation = glm::conjugate(m_qOrientation);
}
