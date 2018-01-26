#include "core/data/spotlight.h"

#include "core/data/shader.h"
#include "core/data/framebuffer.h"

#include <glm/gtc/matrix_transform>

SpotLight::SpotLight(const glm::vec3 &vColor, const GLfloat &fIntensity, const glm::vec3 &vPosition, const glm::vec3 &vDirection, const GLfloat &fInnerCutOff, const GLfloat &fOuterCutOff) :
    ALight(vColor, fIntensity),
    m_vPosition(vPosition),
    m_vDirection(vDirection),
    m_fInnerCutOff(glm::cos(glm::radians(fInnerCutOff))),
    m_fOuterCutOff(glm::cos(glm::radians(fOuterCutOff)))
{
    //  Init Shadow Map
    m_pDepthMapBuffer->init(m_uiDepthMapWidth, m_uiDepthMapHeight);
    const GLfloat afBorderColor[] = {1.f, 1.f, 1.f, 1.f};
    std::vector<TextureAttachment> aTexturesAttachments;
    aTexturesAttachments.push_back({new Texture(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, afBorderColor, GL_TRUE), GL_DEPTH_ATTACHMENT});
    m_pDepthMapBuffer->attachTextures(aTexturesAttachments);

    const glm::mat4 &mLightProjection = glm::perspective(m_fInnerCutOff, m_fDepthMapAspect, m_fDepthMapNear, m_fDepthMapFar);
    const glm::mat4 &mLightView = glm::lookAt(m_vPosition, m_vDirection, glm::vec3(0.f, 1.f, 0.f));
    m_mLightSpaceMatrix = mLightProjection * mLightView;
}
