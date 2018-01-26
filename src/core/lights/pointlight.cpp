#include "core/lights/pointlight.h"

#include "core/framebuffer.h"
#include "core/texture.h"

#include <glm/gtc/matrix_transform.hpp>

PointLight::PointLight(const glm::vec3 &vColor, const GLfloat &fIntensity, const glm::vec3 &vPosition) :
    ALight(vColor, fIntensity),
    m_vPosition(vPosition)
{
    //  Init Shadow Map
    m_pDepthMapBuffer->init(m_uiDepthMapWidth, m_uiDepthMapHeight);
    const GLfloat afBorderColor[] = {1.f, 1.f, 1.f, 1.f};
    std::vector<TextureAttachment> aTexturesAttachments;
    aTexturesAttachments.push_back({new Texture(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, afBorderColor, GL_TRUE, GL_TEXTURE_CUBE_MAP), GL_DEPTH_ATTACHMENT});
    m_pDepthMapBuffer->attachTextures(aTexturesAttachments);


    const glm::mat4 mShadowProj = glm::perspective(glm::radians(90.f), m_fDepthMapAspect, m_fDepthMapNear, m_fDepthMapFar);
    m_amShadowTransforms[0] = glm::mat4(mShadowProj * glm::lookAt(m_vPosition, m_vPosition + glm::vec3( 1.f,  0.f,  0.f), glm::vec3(0.f, -1.f,  0.f)));
    m_amShadowTransforms[1] = glm::mat4(mShadowProj * glm::lookAt(m_vPosition, m_vPosition + glm::vec3(-1.f,  0.f,  0.f), glm::vec3(0.f, -1.f,  0.f)));
    m_amShadowTransforms[2] = glm::mat4(mShadowProj * glm::lookAt(m_vPosition, m_vPosition + glm::vec3( 0.f,  1.f,  0.f), glm::vec3(0.f,  0.f,  1.f)));
    m_amShadowTransforms[3] = glm::mat4(mShadowProj * glm::lookAt(m_vPosition, m_vPosition + glm::vec3( 0.f, -1.f,  0.f), glm::vec3(0.f,  0.f, -1.f)));
    m_amShadowTransforms[4] = glm::mat4(mShadowProj * glm::lookAt(m_vPosition, m_vPosition + glm::vec3( 0.f,  0.f,  1.f), glm::vec3(0.f, -1.f,  0.f)));
    m_amShadowTransforms[5] = glm::mat4(mShadowProj * glm::lookAt(m_vPosition, m_vPosition + glm::vec3( 0.f,  0.f, -1.f), glm::vec3(0.f, -1.f,  0.f)));
}
