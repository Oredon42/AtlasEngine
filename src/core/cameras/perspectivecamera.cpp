#include "core/cameras/perspectivecamera.h"

#include <glm/gtc/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera() :
    m_fFov(45.5f)
{
    updateProjection();
}

PerspectiveCamera::PerspectiveCamera(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp, const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fFov, const GLfloat &fAspect) :
    ACamera(vPosition, vFront, vUp, fNear, fFar),
    m_fFov(fFov),
    m_fAspect(fAspect)
{
    updateProjection();
}

void PerspectiveCamera::setFov(const GLfloat &fFov)
{
    m_fFov = fFov;

    updateProjection();
}

void PerspectiveCamera::setAspect(const GLfloat &fAspect)
{
    m_fAspect = fAspect;

    updateProjection();
}

void PerspectiveCamera::setProjectionData(const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fFov, const GLfloat &fAspect)
{
    m_fNear = fNear;
    m_fFar = fFar;
    m_fFov = fFov;
    m_fAspect = fAspect;

    updateProjection();
}

void PerspectiveCamera::updateProjection()
{
    m_mProjection = glm::perspective(m_fFov, m_fAspect, m_fNear, m_fFar);
}
