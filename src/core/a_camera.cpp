#include "core/a_camera.h"

#include <glm/gtc/matrix_transform.hpp>

ACamera::ACamera(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp, const GLfloat &fNear, const GLfloat &fFar) :
    m_vPosition(vPosition),
    m_vFront(vFront),
    m_vUp(vUp),
    m_fNear(fNear),
    m_fFar(fFar)
{
    updateView();
}

void ACamera::setPosition(const glm::vec3 &vPosition)
{
    m_vPosition = vPosition;
    updateView();
}

void ACamera::setOrientation(const glm::vec3 &vFront, const glm::vec3 &vUp)
{
    m_vFront = vFront;
    m_vUp = vUp;
    updateView();
}

void ACamera::setViewData(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp)
{
    m_vPosition = vPosition;
    m_vFront = vFront;
    m_vUp = vUp;
    updateView();
}

void ACamera::setNear(const GLfloat &fNear)
{
    m_fNear = fmin(fNear, m_fFar);
    updateProjection();
}

void ACamera::setFar(const GLfloat &fFar)
{
    m_fFar = fmax(m_fNear, fFar);
    updateProjection();
}

void ACamera::updateView()
{
    m_mView = glm::lookAt(m_vPosition, m_vPosition + m_vFront, m_vUp);
    m_mTransposedInvertedView = glm::transpose(glm::inverse(glm::mat3(m_mView)));
}
