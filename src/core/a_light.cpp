#include "core/a_light.h"

#include "core/framebuffer.h"

ALight::ALight(const glm::vec3 &vColor, const GLfloat &fIntensity, const GLuint &uiDepthMapWidth, const GLuint &uiDepthMapHeight, const GLfloat &fDepthMapNear, const GLfloat &fDepthMapFar)
{
    m_vColor = glm::clamp(m_vColor, glm::vec3(0, 0, 0), glm::vec3(1, 1, 1));
    m_fIntensity = fIntensity;
    m_pDepthMapBuffer = new Framebuffer();
    m_uiDepthMapWidth = uiDepthMapWidth;
    m_uiDepthMapHeight = uiDepthMapHeight;
    m_fDepthMapNear = fDepthMapNear;
    m_fDepthMapFar = fDepthMapFar;
    m_fDepthMapAspect = m_uiDepthMapWidth / m_uiDepthMapHeight;
}

ALight::~ALight()
{
    delete m_pDepthMapBuffer;
}

void ALight::bindDepthMapBuffer()
{
    m_pDepthMapBuffer->bind();
}

void ALight::releaseDepthMapBuffer()
{
    m_pDepthMapBuffer->release();
}
