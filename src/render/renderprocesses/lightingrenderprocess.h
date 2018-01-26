#ifndef LIGHTINGPROCESS_H
#define LIGHTINGPROCESS_H

#include "a_renderprocess.h"

class Shader;

/*
 * IN:
 * -position_depth          : GL_RGBA16F
 * -normal                  : GL_RGB16F
 * -albedo_spec             : GL_RGBA16F
 * -roughness_ior_metalness : GL_RGB16F
 * -ambient_occlusion       : GL_R16F
 *
 * OUT:
 * -fragment_color          : GL_RGB16F
 * -bright_color            : GL_RGB16F
 * -brightness              : GL_R16F
 * */

class LightingRenderProcess : public ARenderProcess
{
public:
    LightingRenderProcess();
    virtual ~LightingRenderProcess();

    virtual void init(const GLsizei &iWidth, const GLsizei &iHeight);

    virtual void resize(const GLsizei &iWidth, const GLsizei &iHeight);
    virtual void process(const RenderingQuad *pRenderingQuad, const GLfloat &fRenderTime);

    virtual void reloadShaders();

private:
    Framebuffer *m_pOutBuffer;

    Shader *m_pLightingShader;
    Shader *m_pTexture2dDepthShader;
    Shader *m_pCubemapTextureDepthShader;

    //  Private methods
    virtual void allocate();
    void processShadows(const GLfloat &fRenderTime);
};

#endif // LIGHTINGPROCESS_H
