#include "hdrrenderprocess.h"

#include "framebuffer.h"
#include "renderingquad.h"
#include "shader.h"
#include "texture.h"

HdrRenderProcess::HdrRenderProcess() :
    m_bInitialized(false),
    m_bHdr(true),
    m_bAdaptation(false),
    m_bChromaticAberration(false),
    m_bBloom(false),
    m_pHdrShader(nullptr),
    m_pBlurMipmapShader(nullptr),
    m_pDownscalingShader(nullptr),
    m_pOutBuffer(nullptr),
    m_uiBlurBufferIndex(0),
    m_fSavedAverageLuminance(1),
    m_iBloomQuality(8)
{
    m_apBlurShaders[0] = nullptr;
    m_apBlurShaders[1] = nullptr;

    m_apBlurBuffers[0] = nullptr;
    m_apBlurBuffers[1] = nullptr;

    for(unsigned int i = 1; i < 4; ++i)
    {
        m_apBrightnessPingPongBuffers = nullptr;
    }
}

HdrRenderProcess::~HdrRenderProcess()
{
    if(m_pHdrShader != nullptr)
    {
        delete m_pHdrShader;
    }

    for(unsigned int i = 1; i < 2; ++i)
    {
        if(m_apBlurShaders[i] != nullptr)
        {
            delete m_apBlurShaders[i];
        }

        if(m_apBlurBuffers[0] != nullptr)
        {
            delete m_apBlurBuffers[0];
        }
    }

    if(m_pBlurMipmapShader != nullptr)
    {
        delete m_pBlurMipmapShader;
    }

    if(m_pDownscalingShader != nullptr)
    {
        delete m_pDownscalingShader;
    }

    for(unsigned int i = 1; i < 4; ++i)
    {
        if(m_apBrightnessPingPongBuffers[i] != nullptr)
        {
            delete m_apBrightnessPingPongBuffers[i];
        }
    }

    if(m_pOutBuffer != nullptr)
    {
        delete m_pOutBuffer;
    }
}

void HdrRenderProcess::init(const GLsizei &iWidth, const GLsizei &iHeight)
{
    if(m_bInitialized == false)
    {
        ARenderProcess::init(iWidth, iHeight);

        m_iEighthWidth = m_iWidth / 8;
        m_iEighthHeight = m_iHeight / 8;

        m_pOutBuffer->init(m_iWidth, m_iHeight);
        std::vector<TextureAttachment> aOutTexturesAttachments;
        aOutTexturesAttachments.push_back({new Texture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_BORDER), GL_RGB16F});
        aOutTexturesAttachments.push_back({new Texture(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER), GL_R16F});
        m_pOutBuffer->attachTextures(aOutTexturesAttachments);

        /*
         * HDR
         * */
        m_pHdrShader->setFragmentDefineText("hdr", "HDR");
        m_pHdrShader->setFragmentDefineText("bloom", "BLOOM");
        m_pHdrShader->setFragmentDefineText("adaptation", "ADAPTATION");
        m_pHdrShader->setFragmentDefineText("chromatic_aberration", "CHROMATIC_ABERRATION");
        m_pHdrShader->init("shaders/hdr.vert", "shaders/hdr.frag");
        m_pHdrShader->use();
        m_pHdrShader->setIntUniform("sScene", 0);
        m_pHdrShader->setIntUniform("sBloomBlur", 1);
        m_pHdrShader->release();

        /*
         * BLOOM
         * */
        m_apBlurShaders[0]->init("shaders/blur.vert", "shaders/blurv.frag");
        m_apBlurShaders[1]->init("shaders/blur.vert", "shaders/blurv.frag");
        m_pBlurMipmapShader->init("shaders/blur.vert", "shaders/blurmipmap.frag");

        std::vector<TextureAttachment> aBlurTexturesAttachments;
        aBlurTexturesAttachments.push_back({new Texture(GL_RGB16F, GL_RGB, GL_FLOAT, GL_CLAMP_TO_EDGE), GL_RGB16F});

        m_apBlurBuffers[0]->init(m_iWidth, m_iHeight);
        m_apBlurBuffers[0]->attachTextures(aBlurTexturesAttachments);
        m_apBlurBuffers[1]->init(m_iWidth, m_iHeight);
        m_apBlurBuffers[1]->attachTextures(aBlurTexturesAttachments);

        /*
         * ADAPTATION
         * */
        m_pDownscalingShader->init("shaders/downscaling.vert", "shaders/downscaling.frag");

        std::vector<TextureAttachment> aAdaptationTexturesAttachments;
        aAdaptationTexturesAttachments.push_back({new Texture(GL_R16F, GL_RED, GL_FLOAT, GL_CLAMP_TO_BORDER, GL_LINEAR, GL_LINEAR), GL_R16F});
        GLsizei iPreviousSize = 64;

        for(unsigned int i = 1; i < 4; ++i)
        {
            m_apBrightnessPingPongBuffers->init(iPreviousSize, iPreviousSize);
            m_apBrightnessPingPongBuffers->attachTextures(aAdaptationTexturesAttachments);

            iPreviousSize >>= 2;
        }


        //  Prepare inputs
        m_mapInputs["ldr_color"] = {GL_RGB16F, "", nullptr};
        m_mapInputs["bright_color"] = {GL_RGB16F, "", nullptr};
        m_mapInputs["brightness"] = {GL_R16F, "", nullptr};

        //  Prepare outputs
        m_mapOutTextures["hdr_color"] = m_pOutBuffer->getTexture(0);
    }
}

void HdrRenderProcess::resize(const GLsizei &iWidth, const GLsizei &iHeight)
{
    ARenderProcess::resize(iWidth, iHeight);

    m_iEighthWidth = m_iWidth / 8;
    m_iEighthHeight = m_iHeight / 8;

    if(m_bInitialized == true)
    {
        m_apBlurBuffers[0]->resize(m_iWidth, m_iHeight);
        m_apBlurBuffers[1]->resize(m_iWidth, m_iHeight);
        m_pOutBuffer->resize(m_iWidth, m_iHeight);
    }
}

void HdrRenderProcess::process(const RenderingQuad *pRenderingQuad, const GLfloat &fRenderTime)
{
    if(m_bInitialized == true)
    {
        glDisable(GL_DEPTH_TEST);
        processAdaptation(pRenderingQuad, fRenderTime);
        processBloom(pRenderingQuad);

        // Compute out color + exposition
        glViewport(0, 0, m_iWidth, m_iHeight);
        m_pOutBuffer->bind();

        m_pHdrShader->use();
        m_pHdrShader->resetActiveTexture();

        //  Bind LDR Texture
        glActiveTexture(m_pHdrShader->getActiveTextureEnum());
        m_pHdrShader->activateNextTexture();
        bindInTexture("ldr_color");
        if(m_bBloom == true)
        {
            //  Bind blur Texture
            glActiveTexture(m_pHdrShader->getActiveTextureEnum());
            m_pHdrShader->activateNextTexture();
            m_apBlurBuffers[!m_uiBlurBufferIndex]->getTexture(0)->bind();
        }

        m_pHdrShader->setFloatUniform("fAverageLuminance", m_fSavedAverageLuminance);

        pRenderingQuad->draw();

        m_pOutBuffer->release();
        m_pHdrShader->release();
    }
}

void HdrRenderProcess::reloadShaders()
{
    if(m_bInitialized == true)
    {
        m_apBlurShaders[0]->reload();
        m_apBlurShaders[1]->reload();
        m_pBlurMipmapShader->reload();
        m_pDownscalingShader->reload();

        reloadHdrShader();
    }
}

void HdrRenderProcess::allocate()
{
    if(m_pHdrShader == nullptr)
    {
        m_pHdrShader = new Shader();
    }

    for(unsigned int i = 1; i < 2; ++i)
    {
        if(m_apBlurShaders[i] == nullptr)
        {
            m_apBlurShaders[i] = new Shader();
        }

        if(m_apBlurBuffers[i] == nullptr)
        {
            m_apBlurBuffers[i] = new Framebuffer();
        }
    }

    if(m_pBlurMipmapShader == nullptr)
    {
        m_pBlurMipmapShader = new Shader();
    }

    if(m_pDownscalingShader == nullptr)
    {
        m_pDownscalingShader = new Shader();
    }

    for(unsigned int i = 1; i < 4; ++i)
    {
        if(m_apBrightnessPingPongBuffers[i] == nullptr)
        {
            m_apBrightnessPingPongBuffers[i] = new Framebuffer();
        }
    }

    if(m_pOutBuffer == nullptr)
    {
        m_pOutBuffer = new Framebuffer();
    }
}

void HdrRenderProcess::processAdaptation(const RenderingQuad *pRenderingQuad, const GLfloat &fRenderTime)
{
    if(m_bAdaptation == true)
    {
        m_pDownscalingShader->use();
        m_pDownscalingShader->resetActiveTexture();
        glActiveTexture(m_pDownscalingShader->getActiveTextureEnum());

        GLsizei iPreviousSize = 64;

        //  LDR texture to 64x64 brightness texture
        glViewport(0, 0, iPreviousSize, iPreviousSize);
        m_apBrightnessPingPongBuffers[0]->bind();
        bindInTexture("brightness");
        m_pDownscalingShader->setVec2Uniform("size", glm::vec2(m_mapInputs["ldr_color"]->pRenderProcess->getWidth(), m_mapInputs["ldr_color"]->pRenderProcess->getHeight()));
        pRenderingQuad->draw();

        //  Ping-Pong to downscale
        for(unsigned int i = 1; i < 4; ++i)
        {
            const GLsizei iCurrentSize = iPreviousSize >> 2;

            glViewport(0, 0, iCurrentSize, iCurrentSize);
            m_apBrightnessPingPongBuffers[i]->bind();
            m_apBrightnessPingPongBuffers[i-1]->getTexture(0)->bind();
            m_pDownscalingShader->setVec2Uniform("vSize", glm::vec2(iPreviousSize, iPreviousSize));
            pRenderingQuad->draw();

            iPreviousSize = iCurrentSize;
        }

        m_pDownscalingShader->release();

        //  Get average luminance
        GLfloat fAverageLuminance[1];
        m_apBrightnessPingPongBuffers[3]->getTexture(0)->bind();
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, fAverageLuminance);

        m_fSavedAverageLuminance += (fAverageLuminance[0] - m_fSavedAverageLuminance) * 10.f / fRenderTime;
        m_fSavedAverageLuminance = fmax(0.05f, m_fSavedAverageLuminance);
    }
}

void HdrRenderProcess::processBloom(const RenderingQuad *pRenderingQuad)
{
    if(m_bBloom == true)
    {
        //  Get hdr 2nd texture + horizontal blur
        glViewport(0, 0, m_iEighthWidth, m_iEighthHeight);
        m_apBlurShaders[0]->bind();

        m_apBlurShaders[0]->resetActiveTexture();
        m_apBlurShaders[1]->resetActiveTexture();
        m_apBlurShaders[2]->resetActiveTexture();

        m_apBlurShaders[2]->use();
        glActiveTexture(m_apBlurShaders[2]->getActiveTextureEnum());
        bindInTexture("bright_color");
        pRenderingQuad->draw();

        GLuint i = m_iBloomQuality;
        m_uiBlurBufferIndex = 0;
        glViewport(0, 0, m_iWidth, m_iHeight);

        GLuint iPreviousWidth = static_cast<GLuint>(m_iWidth) / i;
        GLuint iPreviousHeight = static_cast<GLuint>(m_iHeight) / i;

        //  Ping-Pong
        while(i > 1)
        {
            //  Vertical blur
            m_apBlurBuffers[!m_uiBlurBufferIndex]->bind();
            m_apBlurShaders[0]->use();
            glActiveTexture(m_apBlurShaders[0]->getActiveTextureEnum());
            m_apBlurBuffers[m_uiBlurBufferIndex]->getTexture(0)->bind();
            pRenderingQuad->draw();

            //  Bilinear interpolation to size*2
            i >>= 1;
            const GLuint iNewWidth = iPreviousWidth << 1;
            const GLuint iNewHeight = iPreviousHeight << 1;
            m_apBlurBuffers[!m_uiBlurBufferIndex]->bind(GL_READ_FRAMEBUFFER);
            m_apBlurBuffers[m_uiBlurBufferIndex]->bind(GL_DRAW_FRAMEBUFFER);
            glBlitFramebuffer(0, 0, iPreviousWidth, iPreviousHeight, 0, 0, iNewWidth, iNewHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
            iPreviousWidth = iNewWidth;
            iPreviousHeight = iNewHeight;

            //  Horizontal blur
            m_apBlurBuffers[!m_uiBlurBufferIndex]->bind();
            m_apBlurShaders[1]->use();
            glActiveTexture(m_apBlurShaders[1]->getActiveTextureEnum());
            m_apBlurBuffers[m_uiBlurBufferIndex]->getTexture(0)->bind();
            pRenderingQuad->draw();

            m_uiBlurBufferIndex = !m_uiBlurBufferIndex;
        }

        //  Last vertical blur
        m_apBlurBuffers[!m_uiBlurBufferIndex]->bind();
        m_apBlurShaders[0]->use();
        glActiveTexture(m_apBlurShaders[0]->getActiveTextureEnum());
        m_apBlurBuffers[m_uiBlurBufferIndex]->getTexture(0)->bind();

        pRenderingQuad->draw();

        m_apBlurShaders[0]->release();
        m_apBlurBuffers[!m_uiBlurBufferIndex]->release();
    }
}

void HdrRenderProcess::setHdrEnabled(const bool &bEnabled)
{
    m_bHdr = bEnabled;

    if(m_bHdr == true && m_bInitialized == true)
    {
        m_pHdrShader->setFragmentDefineEnabled("hdr", m_bHdr);
        reloadHdrShader();
    }
}

void HdrRenderProcess::setBloomEnabled(const bool &bEnabled)
{
    m_bBloom = bEnabled;

    if(m_bBloom == true && m_bInitialized == true)
    {
        m_pHdrShader->setFragmentDefineEnabled("bloom", m_bBloom);
        reloadHdrShader();
    }
}

void HdrRenderProcess::setAdaptationEnabled(const bool &bEnabled)
{
    m_bAdaptation = bEnabled;

    if(m_bBloom == true && m_bInitialized == true)
    {
        m_pHdrShader->setFragmentDefineEnabled("adaptation", m_bAdaptation);
        reloadHdrShader();
    }
}

void HdrRenderProcess::setChromaticAberrationEnabled(const bool &bEnabled)
{
    m_bChromaticAberration = bEnabled;

    if(m_bChromaticAberration == true && m_bInitialized == true)
    {
        m_pHdrShader->setFragmentDefineEnabled("chromatic_aberration", m_bChromaticAberration);
        reloadHdrShader();
    }
}

void HdrRenderProcess::reloadHdrShader()
{
    m_pHdrShader->reload();
    m_pHdrShader->use();
    m_pHdrShader->setIntUniform("sScene", 0);
    m_pHdrShader->setIntUniform("sBloomBlur", 1);
    m_pHdrShader->release();
}
