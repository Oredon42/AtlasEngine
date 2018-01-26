#ifndef HDRRENDERPROCESS_H
#define HDRRENDERPROCESS_H

#include "a_renderprocess.h"

class Shader;

/*
 * IN:
 * -ldr_color       : GL_RGB16F
 * -bright_color    : GL_RGB16F
 * -brightness      : GL_R16F
 *
 * OUT:
 * -hdr_color       : GL_RGB16F
 * */
class HdrRenderProcess : public ARenderProcess
{
public:
    HdrRenderProcess();
    virtual ~HdrRenderProcess();

    virtual void init(const GLsizei &iWidth, const GLsizei &iHeight);

    virtual void resize(const GLsizei &iWidth, const GLsizei &iHeight);
    virtual void process(const RenderingQuad *pRenderingQuad, const GLfloat &fRenderTime);
    virtual void reloadShaders();

    //  Setters
    virtual void setActivated(const GLboolean &activated);

    void setHdrEnabled(const bool &bEnabled);
    void setBloomEnabled(const bool &bEnabled);
    void setAdaptationEnabled(const bool &bEnabled);
    void setChromaticAberrationEnabled(const bool &bEnabled);

private:
    GLsizei m_iEighthWidth;
    GLsizei m_iEighthHeight;

    bool m_bHdr;
    bool m_bBloom;
    bool m_bAdaptation;
    bool m_bChromaticAberration;

    Shader *m_pHdrShader;
    Shader *m_apBlurShaders[2];
    Shader *m_pBlurMipmapShader;
    Shader *m_pDownscalingShader;

    Framebuffer *m_apBlurBuffers[2];
    Framebuffer *m_apBrightnessPingPongBuffers[4];
    Framebuffer *m_pOutBuffer;

    GLfloat m_fSavedAverageLuminance;

    unsigned int m_uiBlurBufferIndex;

    GLsizei m_iBloomQuality;

    //  Private methods
    virtual void allocate();
    void processAdaptation(const RenderingQuad *pRenderingQuad, const GLfloat &fRenderTime);
    void processBloom(const RenderingQuad *pRenderingQuad);
    void reloadHdrShader();
};

#endif // HDRRENDERPROCESS_H
