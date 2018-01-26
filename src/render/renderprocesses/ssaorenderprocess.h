#ifndef SSAORENDERPROCESS_H
#define SSAORENDERPROCESS_H

#include "glmincludes.h"

#include "a_renderprocess.h"

class Shader;

/*
 * IN:
 * -position_depth      : GL_RGBA16F
 * -normal              : GL_RGB16F
 *
 * OUT:
 * -ambient_occlusion   : GL_R16F
 * */

class SsaoRenderProcess : public ARenderProcess
{
private:
    //  Static methods
    inline static GLfloat lerp(const GLfloat &a, const GLfloat &b, const GLfloat &f) {return a + f * (b - a);}

public:
    SsaoRenderProcess();
    virtual ~SsaoRenderProcess();

    virtual void init(const GLsizei &iWidth, const GLsizei &iHeight);
    virtual void resize(const GLsizei &iWidth, const GLsizei &iHeight);

    void process(const RenderingQuad *pRenderingQuad, const float &fRenderTime);

    virtual void reloadShaders();

private:
    GLuint m_uiNumSamples;

    Framebuffer *m_pSsaoBuffer;
    Framebuffer *m_pSsaoBlurBuffer;

    Shader *m_pSsaoShader;
    Shader *m_pSsaoBlurShader;

    Texture *m_pNoiseTexture;

    std::vector<glm::vec3> m_avSsaoKernel;
    std::vector<std::string> m_astrKernelUniformNames;

    //  Private methods
    virtual void allocate();
    void generateNoise();
};

#endif // SSAORENDERPROCESS_H
