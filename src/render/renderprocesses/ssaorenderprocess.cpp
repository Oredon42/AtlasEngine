#include "ssaorenderprocess.h"

#include "framebuffer.h"
#include "renderingquad.h"
#include "shader.h"
#include "texture.h"

#include <random>

SsaoRenderProcess::SsaoRenderProcess() :
    m_uiNumSamples(64),
    m_pSsaoBuffer(nullptr),
    m_pSsaoBlurBuffer(nullptr),
    m_pSsaoShader(nullptr),
    m_pSsaoBlurShader(nullptr),
    m_pNoiseTexture(nullptr)
{    

}

SsaoRenderProcess::~SsaoRenderProcess()
{
    if(m_pSsaoBuffer != nullptr)
    {
        delete m_pSsaoBuffer;
    }

    if(m_pSsaoBlurBuffer != nullptr)
    {
        delete m_pSsaoBlurBuffer;
    }

    if(m_pSsaoShader != nullptr)
    {
        delete m_pSsaoShader;
    }

    if(m_pSsaoBlurShader != nullptr)
    {
        delete m_pSsaoBlurShader;
    }

    if(m_pNoiseTexture != nullptr)
    {
        delete m_pNoiseTexture;
    }
}

void SsaoRenderProcess::init(const GLsizei &iWidth, const GLsizei &iHeight)
{
    if(m_bInitialized == false)
    {
        ARenderProcess::init(iWidth, iHeight);

        m_pSsaoShader = new Shader("shaders/ssao.vert", "shaders/ssao.frag");

        m_pSsaoBlurShader = new Shader("shaders/ssao.vert", "shaders/ssaoblur.frag");
        reloadSsaoBlurShader();

        std::vector<TextureAttachment> aBlurTexturesAttachments;
        aBlurTexturesAttachments.push_back({new Texture(GL_R16F, GL_RED, GL_FLOAT), GL_R16F});

        m_pSsaoBuffer = new Framebuffer();
        m_pSsaoBuffer->init(m_iWidth / 2, m_iHeight / 2);
        m_pSsaoBuffer->attachTextures(aBlurTexturesAttachments);

        m_pSsaoBlurBuffer = new Framebuffer();
        m_pSsaoBlurBuffer->init(iWidth, iHeight);
        m_pSsaoBlurBuffer->attachTextures(aBlurTexturesAttachments);

        m_pNoiseTexture = new Texture();
        generateNoise();

        m_astrKernelUniformNames.reserve(m_uiNumSamples);
        for(GLuint i = 0; i < m_uiNumSamples; ++i)
        {
            m_astrKernelUniformNames.push_back("samples[" + std::to_string(i) + "]");
        }

        //  Prepare inputs
        m_mapInputs["position_depth"] = {GL_RGBA16F, "", nullptr};
        m_mapInputs["normal"] = {GL_RGB16F, "", nullptr};

        //  Prepare outputs
        m_mapOutTextures["ambient_occlusion"] = m_pSsaoBlurBuffer->getTexture(0);
    }
}

void SsaoRenderProcess::resize(const GLsizei &iWidth, const GLsizei &iHeight)
{
    ARenderProcess::resize(iWidth, iHeight);

    if(m_bInitialized == true)
    {
        m_pSsaoBuffer->resize(m_iWidth / 2, m_iHeight / 2);
        m_pSsaoBlurBuffer->resize(m_iWidth, m_iHeight);
    }
}

void SsaoRenderProcess::process(const RenderingQuad *pRenderingQuad, const float &fRenderTime)
{
    if(m_bInitialized == true)
    {
        glViewport(0, 0, m_pSsaoBuffer->getWidth(), m_pSsaoBuffer->getHeight());
        m_pSsaoBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);

        m_pSsaoShader->use();
        m_pSsaoShader->resetActiveTexture();

        glActiveTexture(m_pSsaoShader->getActiveTextureEnum());
        m_pSsaoShader->activateNextTexture();
        bindInTexture("position_depth");
        glActiveTexture(m_pSsaoShader->getActiveTextureEnum());
        m_pSsaoShader->activateNextTexture();
        bindInTexture("normal");
        glActiveTexture(m_pSsaoShader->getActiveTextureEnum());
        m_pSsaoShader->activateNextTexture();
        m_pNoiseTexture->bind();

        for(GLuint i = 0; i < m_uiNumSamples; ++i)
        {
            m_pSsaoShader->setVec3Uniform(m_astrKernelUniformNames, &m_avSsaoKernel[i]);
        }

        m_pSsaoShader->setMat4Uniform("mView", scene.getCurrentCamera()->getView());
        m_pSsaoShader->setMat3Uniform("mTransposedInverseView", scene.getCurrentCamera()->getTransposedInvertedView());
        m_pSsaoShader->setMat4Uniform("mProjection", scene.getCurrentCamera()->getProjection());
        m_pSsaoShader->setVec2Uniform("vWindowSize", glm::vec2(m_pSsaoBuffer->getWidth(), m_pSsaoBuffer->getHeight()));
        pRenderingQuad->draw();

        glViewport(0, 0, m_pSsaoBlurBuffer->getWidth(), m_pSsaoBlurBuffer->getHeight());
        m_pSsaoBlurBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT);

        m_pSsaoBlurShader->use();
        m_pSsaoBlurShader->resetActiveTexture();
        glActiveTexture(m_pSsaoBlurShader->getActiveTexture());
        m_pSsaoBuffer->getTexture(0)->bind();
        pRenderingQuad->draw();

        m_pSsaoBlurShader->release();

        m_pSsaoBuffer->release();
    }
}

void SsaoRenderProcess::reloadShaders()
{
    if(m_bInitialized == true)
    {
        m_pSsaoShader->reload();
        m_pSsaoBlurShader->reload();
        m_pSsaoBlurShader->use();
        m_pSsaoBlurShader->setIntUniform("sPositionDepth", 0);
        m_pSsaoBlurShader->setIntUniform("sNormal", 1);
        m_pSsaoBlurShader->setIntUniform("sTexNoise", 2);
        m_pSsaoBlurShader->release();
    }
}

void SsaoRenderProcess::allocate()
{
    if(m_pSsaoBuffer == nullptr)
    {
        m_pSsaoBuffer = new Framebuffer();
    }

    if(m_pSsaoBlurBuffer == nullptr)
    {
        m_pSsaoBlurBuffer = new Framebuffer();
    }

    if(m_pSsaoShader == nullptr)
    {
        m_pSsaoShader = new Shader();
    }

    if(m_pSsaoBlurShader == nullptr)
    {
        m_pSsaoBlurShader = new Shader();
    }

    if(m_pNoiseTexture == nullptr)
    {
        m_pNoiseTexture = new Texture();
    }
}

/*
 * Generate 4*4 repeated noise Texture
 * */
void SsaoRenderProcess::generateNoise()
{
    if(m_bInitialized == true)
    {
        m_avSsaoKernel.clear();
        m_avSsaoKernel.reserve(m_uiNumSamples);

        const std::uniform_real_distribution<GLfloat> randomFloats(0.f, 1.f);
        const std::default_random_engine generator;

        const float fInverseSize = 1.f / static_cast<float>(m_uiNumSamples);

        //  Samples
        for(GLuint i = 0; i < m_uiNumSamples; ++i)
        {
            const float fScale = static_cast<float>(i) * fInverseSize;
            const float fInterpolatedSscale = lerp(0.1f, 1.0f, fScale * fScale);

            const float x = randomFloats(generator) * 2.0 - 1.0;
            const float y = randomFloats(generator) * 2.0 - 1.0;
            const float &z = randomFloats(generator);

            const glm::vec3 vSample = glm::normalize(glm::vec3(x, y, z)) * randomFloats(generator) * fInterpolatedSscale;

            m_avSsaoKernel.push_back(vSample);
        }

        // Noise texture
        std::vector<glm::vec3> avSsaoNoise();
        avSsaoNoise.reserve(16);

        for(GLuint i = 0; i < 16; ++i)
        {
            const float x = randomFloats(generator) * 2.0 - 1.0;
            const float y = randomFloats(generator) * 2.0 - 1.0;

            const glm::vec3 vNoise(x, y, 0.0f);

            avSsaoNoise.push_back(vNoise);
        }

        m_pNoiseTexture.init(GL_RGB16F, 4, 4, GL_RGB, GL_FLOAT, &avSsaoNoise[0], GL_REPEAT, GL_NEAREST, GL_NEAREST);
    }
}
