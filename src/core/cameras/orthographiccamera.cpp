#include "core/cameras/orthographiccamera.h"

#include <glm/gtc/matrix_transform.hpp>

OrthographicCamera::OrthographicCamera() :
    m_fLeft(10),
    m_fRight(10),
    m_fBottom(10),
    m_fTop(10)
{
    updateProjection();
}

OrthographicCamera::OrthographicCamera(const glm::vec3 &vPosition, const glm::vec3 &vFront, const glm::vec3 &vUp, const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fLeft, const GLfloat &fRight, const GLfloat &fBottom, const GLfloat &fTop) :
    ACamera(vPosition, vFront, vUp, fNear, fFar),
    m_fLeft(fLeft),
    m_fRight(fRight),
    m_fBottom(fBottom),
    m_fTop(fTop)
{
    updateProjection();
}

void OrthographicCamera::setDimensions(const GLfloat &fLeft, const GLfloat &fRight, const GLfloat &fBottom, const GLfloat &fTop)
{
    m_fLeft = fLeft;
    m_fRight = fRight;
    m_fBottom = fBottom;
    m_fTop = fTop;

    updateProjection();
}

void OrthographicCamera::setProjectionData(const GLfloat &fNear, const GLfloat &fFar, const GLfloat &fLeft, const GLfloat &fRight, const GLfloat &fBottom, const GLfloat &fTop)
{
    m_fNear = fNear;
    m_fFar = fFar;
    m_fLeft = fLeft;
    m_fRight = fRight;
    m_fBottom = fBottom;
    m_fTop = fTop;

    updateProjection();
}

void OrthographicCamera::updateProjection()
{
    m_mProjection = glm::ortho(m_fLeft, m_fRight, m_fBottom, m_fTop, m_fNear, m_fFar);
}
