#include "core/lights/dirlight.h"

#include "core/framebuffer.h"
#include "core/texture.h"

#include <glm/gtc/matrix_transform.hpp>

DirLight::DirLight(const glm::vec3 &vColor, const GLfloat &fIntensity, const glm::vec3 &vDirection) :
    ALight(vColor, fIntensity),
    m_vDirection(vDirection)
{
    //  Init Shadow Map
    m_pDepthMapBuffer->init(m_uiDepthMapWidth, m_uiDepthMapHeight);
    const GLfloat afBorderColor[] = {1.f, 1.f, 1.f, 1.f};
    std::vector<TextureAttachment> aTexturesAttachments;
    aTexturesAttachments.push_back({new Texture(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, GL_CLAMP_TO_EDGE, GL_NEAREST, GL_NEAREST, afBorderColor, GL_TRUE), GL_DEPTH_ATTACHMENT});
    m_pDepthMapBuffer->attachTextures(aTexturesAttachments);

    const glm::mat4 &mLightProjection = glm::ortho(-150.f, 150.f, -150.f, 150.f, m_fDepthMapNear, m_fDepthMapFar);
    const glm::mat4 &mLightView = glm::lookAt(-m_vDirection * 12.f, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));
    m_mLightSpaceMatrix = mLightProjection * mLightView;
}
